#!/bin/bash
################################################################################
##
## Project : Alarm Clock V3
## File    : tools/tzdata/gen_tzdata.sh
## Author  : Benoit Frigon <www.bfrigon.com>
##
## -----------------------------------------------------------------------------
##
## This work is licensed under the Creative Commons Attribution-ShareAlike 4.0
## International License. To view a copy of this license, visit
##
## http://creativecommons.org/licenses/by-sa/4.0/
##
## or send a letter to Creative Commons,
## PO Box 1866, Mountain View, CA 94042, USA.
##
## Credits : This script parse the IANA timezone database
##           https://www.iana.org/time-zones
##
################################################################################


## Directory where to download the timezone database
DIR_TZDB="./cache"

OUTPUT_TZDATA="src/libs/tzdata.h"

## Timezone files to process
TZ_FILES=(
    "africa"
    "antarctica"
    "asia"
    "australasia"
    "europe"
    "northamerica"
    "southamerica"
    # "etcetera"
    # "factory"
)

## Zones to ignore
ZONES_BLACKLIST="EST;MST;HST;EST5EDT;CST6CDT;MST7MDT;PST8PDT;CET;EET;WET;MET;Etc/UTC"


################################################################################
function parseTime() {

    PARSED_TIME_HOUR=0
    PARSED_TIME_MIN=0

    if [[ "${1:1:1}" == ":" ]]; then 
        PARSED_TIME_HOUR=${1:0:1}
        PARSED_TIME_MIN=${1:2:2}
    elif [[ "${1:2:1}" == ":" ]]; then 
        PARSED_TIME_HOUR=${1:0:2} 
        PARSED_TIME_MIN=${1:3:2}
    elif [[ "${1:3:1}" == ":" ]]; then 
        PARSED_TIME_HOUR=${1:0:3} 
        PARSED_TIME_MIN=${1:4:2}
    else 
        PARSED_TIME_HOUR=${1} 
        PARSED_TIME_MIN=0
    fi

    [[ $PARSED_TIME_HOUR -ge 0 ]] \
        && PARSED_TIME=$(( PARSED_TIME_HOUR * 60 + PARSED_TIME_MIN )) \
        || PARSED_TIME=$(( PARSED_TIME_HOUR * 60 - PARSED_TIME_MIN ))
}


shopt -s nocasematch



##------------------------------------------------------------------------------
##
## Download the IANA timezone database
##
##------------------------------------------------------------------------------

mkdir -p $DIR_TZDB
cd $DIR_TZDB

if [ ! -f version ] || [[ "$1" == "update" ]]; then

    echo "Downloading latest timezone database from IANA..."
    rm -f tzdata-latest.tar.gz

    wget -q --show-progress https://www.iana.org/time-zones/repository/tzdata-latest.tar.gz
    gzip -dc tzdata-latest.tar.gz | tar -xf -
    echo
fi

tzdb_version=$( <version )
printf "Timezone database version: %s\n" $tzdb_version
echo



##------------------------------------------------------------------------------
##
## Parse the database. Discard historical data, only keep the most recent 
## standard time (STD) and daylight saving time (DST) rules for each time zones.
##
##------------------------------------------------------------------------------
declare -A rule_dst
declare -A rule_std
declare -A rule_dst_day
declare -A rule_dst_month
declare -A rule_dst_at
declare -A rule_dst_save
declare -A rule_dst_letter
declare -A rule_std_day
declare -A rule_std_month
declare -A rule_std_at
declare -A rule_std_letter
declare -A zone
declare -A zone_rule
declare -A zone_std_offset
declare -A zone_format
declare -A zone_link

current_year=$(date +'%Y')


for tz_filename in "${TZ_FILES[@]}"; do
    echo "Parsing $tz_filename..."

    if [ ! -f "$tz_filename" ]; then
        echo "[ Database file '$tz_filename' does not exists! ]"
        continue
    fi

    current_zone=""

    while read -ra line; do

        ## Ignore empty lines and comment lines ##
        [ -z "$line" ] && continue
        [[ "$line" =~ ^\#.*$ ]] && continue


        ##--------------------------------------------------------------
        ## Process alias zone record
        ##--------------------------------------------------------------
        if [[ "${line[0]}" == "Link" ]]; then
            zone_link[${line[2]}]=${line[1]}
        
        ##--------------------------------------------------------------
        ## Process time change rule record
        ##--------------------------------------------------------------
        elif [[ "${line[0]}" == "Rule" ]]; then
            
            name=${line[1]}
            from=${line[2]}
            to=${line[3]}
            month=${line[5]}
            day=${line[6]}
            at=${line[7]}
            save=${line[8]}
            letter=${line[9]}

            [[ "$to" == "max" ]] && to=$current_year
            [[ "$to" == "only" ]] && to=$from

            parseTime ${save}

            ## save column is 0 means the rule is for standard time ##
            if [[ $PARSED_TIME -eq 0 ]]; then
                
                ## only keep the most recent STD time rule ##
                if [[ $current_year -le $to ]] && [[ $current_year -ge $from ]]; then

                    rule_std_month[$name]=$month
                    rule_std_day[$name]=$day
                    rule_std_at[$name]=$at
                    rule_std_letter[$name]=$letter

                elif [[ $to -lt $current_year ]]; then
                    rule_std_letter[$name]=$letter
                fi
            else 

                ## only keep the most recent DST time rule ##
                if [[ $current_year -le $to ]] && [[ $current_year -ge $from ]]; then

                    rule_dst_month[$name]=$month
                    rule_dst_day[$name]=$day
                    rule_dst_at[$name]=$at
                    rule_dst_letter[$name]=$letter
                    rule_dst_save[$name]=$save
                fi
            fi

        ##--------------------------------------------------------------
        ## Process a zone record
        ##--------------------------------------------------------------
        else

            if [[ "${line[0]}" == "Zone" ]]; then

                [[ "$ZONES_BLACKLIST" == *"${line[1]}"* ]] && continue

                current_zone=${line[1]}
                
                zone[$current_zone]=${line[3]}
                zone_std_offset[$current_zone]=${line[2]}
                zone_format[$current_zone]=${line[4]}

                continue
            fi

            ## record with the empty 'until' column is the most recent
            if [ -z ${line[3]} ] || [[ "${line[3]:0:1}" == "#" ]]; then

                [ -z "$current_zone" ] && continue

                zone[$current_zone]=${line[1]}
                zone_std_offset[$current_zone]=${line[0]}
                zone_format[$current_zone]=${line[2]}

                current_zone=""
            fi
        fi
    done < $tz_filename
done

##--------------------------------------------------------------
## Add all zone aliases to the zone list
##--------------------------------------------------------------
for name in "${!zone_link[@]}"; do

    link_to=${zone_link[$name]}

    [ -z "${zone[$link_to]}" ] && continue;

    zone[$name]=${zone[$link_to]}
    zone_std_offset[$name]=${zone_std_offset[$link_to]}
    zone_format[$name]=${zone_format[$link_to]}
done

echo
echo "${#zone[@]} zones found"
echo


##------------------------------------------------------------------------------
##
## Generate timezone header files
##
##------------------------------------------------------------------------------
echo "Build timezone table..."

filename_tz_names="__zone_names.h"
filename_abbrev_list="__abbrev_list.h"
filename_tz_table="__tz_table.h"

rm -f $filename_tz_names
rm -f $filename_abbrev_list
rm -f $filename_tz_table


declare -A abbrev_list


for zone_name in "${!zone[@]}"; do

    zone_format=${zone_format[$zone_name]}
    zone_offset=${zone_std_offset[$zone_name]}
    zone_rule=${zone[$zone_name]}

    std_month=${rule_std_month[$zone_rule]^^}
    std_day=${rule_std_day[$zone_rule]}
    std_at=${rule_std_at[$zone_rule]}
    std_letter=${rule_std_letter[$zone_rule]}

    dst_month=${rule_dst_month[$zone_rule]^^}
    dst_day=${rule_dst_day[$zone_rule]}
    dst_at=${rule_dst_at[$zone_rule]}
    dst_letter=${rule_dst_letter[$zone_rule]}
    dst_save=${rule_dst_save[$zone_rule]}

    [[ "$dst_letter" == "-" ]] && dst_letter=""
    [[ "$std_letter" == "-" ]] && std_letter=""


    ##--------------------------------------------------------------
    ## Write the zone name string declaration to the output
    ##--------------------------------------------------------------
    var_zone_name="TZ_${zone_name^^}"
    var_zone_name=${var_zone_name//'/'/'_'}
    var_zone_name=${var_zone_name//'+'/'P'}
    var_zone_name=${var_zone_name//'-'/'_'}

    echo "const char ${var_zone_name^^}[] PROGMEM = { \"$zone_name\" };" >> $filename_tz_names


    ##--------------------------------------------------------------
    ## Add the zone DST abbreviation to the list
    ##--------------------------------------------------------------
    printf -v abbrev_dst -- ${zone_format} ${dst_letter}

    if [[ ! -z "$abbrev_dst" ]]; then 
        var_abbrev_dst="TZ_${abbrev_dst^^}"
        var_abbrev_dst=${var_abbrev_dst//'/'/'_'}
        var_abbrev_dst=${var_abbrev_dst//'+'/'P'}
        var_abbrev_dst=${var_abbrev_dst//'-'/'M'}

        abbrev_list[$var_abbrev_dst]=$abbrev_dst
    else
        var_abbrev_dst="TZ_NULL"
    fi


    ##--------------------------------------------------------------
    ## Add the zone STD abbreviation to the list
    ##--------------------------------------------------------------
    printf -v abbrev_std -- ${zone_format} ${std_letter}

    if [[ ! -z "$abbrev_std" ]]; then 
        var_abbrev_std="TZ_${abbrev_std^^}"
        var_abbrev_std=${var_abbrev_std//'/'/'_'}
        var_abbrev_std=${var_abbrev_std//'+'/'P'}
        var_abbrev_std=${var_abbrev_std//'-'/'M'}

        abbrev_list[$var_abbrev_std]=$abbrev_std
    else
        var_abbrev_std="TZ_NULL"
    fi

    
    ##--------------------------------------------------------------
    ## Parse data from time change rules for the current zone
    ##--------------------------------------------------------------

    ## Parse standard time offset from UTC in minutes
    parseTime $zone_offset
    zone_offset_hour=$(( PARSED_TIME_HOUR ))
    zone_offset_min=$(( PARSED_TIME_MIN ))
    std_offset=$PARSED_TIME
    
    ## Parse standard time starting hour/min
    parseTime $std_at
    std_at_hour=$(( PARSED_TIME_HOUR ))
    std_at_min=$(( PARSED_TIME_MIN ))

    ## Parse daylight saving time offset from UTC in minutes
    parseTime $dst_save

    ## deal with negative DST (IANA why?... just... why?) ##
    if [[ $PARSED_TIME -lt 0 ]]; then
        negative_dst=1
        dst_offset=$(( std_offset - PARSED_TIME ))
    else 
        negative_dst=0
        dst_offset=$(( std_offset + PARSED_TIME ))
    fi
    dst_save_hour=$(( PARSED_TIME_HOUR ))
    dst_save_min=$(( PARSED_TIME_MIN ))

    ## Parse daylight saving time starting hour/min
    parseTime $dst_at
    dst_at_hour=$(( PARSED_TIME_HOUR ))
    dst_at_min=$(( PARSED_TIME_MIN ))

    ## DST to STD transition occurs at STD time rather than DST time
    if [[ "${std_at:(-1): 1}" == "s" ]]; then
        std_at_hour=$(( std_at_hour + dst_save_hour ))
        std_at_min=$(( std_at_min + dst_save_min ))
    fi  

    ## DST to STD transition occurs at UTC time rather than local time
    if [[ "${std_at:(-1): 1}" == "u" ]]; then
        std_at_hour=$(( std_at_hour + zone_offset_hour + dst_save_hour ))
        std_at_min=$(( std_at_min + zone_offset_min + dst_save_min ))
    fi

    ## STD TO DST transition occurs at UTC time rather than local time
    if [[ "${dst_at:(-1): 1}" == "u" ]]; then
        dst_at_hour=$(( dst_at_hour + zone_offset_hour ))
        dst_at_min=$(( dst_at_min + zone_offset_min ))
    fi
    
    ## Parse standard time starting week/day
    if [[ "${std_day:0:4}" == "last" ]]; then 
        std_week=5
        std_day="D_${std_day:4}"
    elif [[ "${std_day:3:2}" == ">=" ]]; then 
        std_week=$(( ${std_day:5} / 7 + 1 ))
        std_day="D_${std_day:0:3}"
    else
        std_week=0
    fi

    ## Parse daylight saving time starting week/day
    if [[ "${dst_day:0:4}" == "last" ]]; then 
        dst_week=5
        dst_day="D_${dst_day:4}"
    elif [[ "${dst_day:3:2}" == ">=" ]]; then 
        dst_week=$(( ${dst_day:5} / 7 + 1 ))
        dst_day="D_${dst_day:0:3}"
    else
        dst_week=0
    fi
  


    ##--------------------------------------------------------------
    ## Write the timezone table entry to the output
    ##--------------------------------------------------------------
    if [[ "$zone_rule" == "-" ]] || [[ -z $std_month ]]; then

        std_def="$std_offset, 0, 0, 0, 0, 0, $var_abbrev_std"
        dst_def=$std_def
    else

        if [[ $negative_dst -eq 1 ]]; then 
            std_def="$std_offset, M_${dst_month^^}, $dst_week, ${dst_day^^}, $dst_at_hour, $dst_at_min, $var_abbrev_dst"
            dst_def="$dst_offset, M_${std_month^^}, $std_week, ${std_day^^}, $std_at_hour, $std_at_min, $var_abbrev_std"
        else
            std_def="$std_offset, M_${std_month^^}, $std_week, ${std_day^^}, $std_at_hour, $std_at_min, $var_abbrev_std"
            dst_def="$dst_offset, M_${dst_month^^}, $dst_week, ${dst_day^^}, $dst_at_hour, $dst_at_min, $var_abbrev_dst"
        fi
    fi

    echo "    { $var_zone_name, $std_def, $dst_def }," >> $filename_tz_table
done


##--------------------------------------------------------------
## Write the zone abbreviation string declarations to the output
##--------------------------------------------------------------
for name in "${!abbrev_list[@]}"; do
    echo "const char $name[] PROGMEM = { \"${abbrev_list[$name]}\" };" >> $filename_abbrev_list
done

echo "Sorting table entries..."
sort -o $filename_tz_names $filename_tz_names
sort -o $filename_abbrev_list $filename_abbrev_list
sort -o $filename_tz_table $filename_tz_table


##--------------------------------------------------------------
## Assemble the timezone data header file
##--------------------------------------------------------------

output="../../../$OUTPUT_TZDATA"
header_name="${OUTPUT_TZDATA##*/}"
header_name="${header_name//'.'/'_'}"
header_name="${header_name^^}"


echo "//******************************************************************************
//
// Project : Alarm Clock V3
// File    : $OUTPUT_TZDATA
// Author  : Benoit Frigon <www.bfrigon.com>
//
// -----------------------------------------------------------------------------
//
//                           [ Important! ]
//
// This file is auto-generated by the script gen_tzdata.sh in /tools/tzdata. 
// This scripts download the timezone database from IANA and generates the time 
// zone table bellow
//
// DO NOT EDIT ANYTHING IN THIS FILE AS IT WILL BE OVERWRITTEN
//
// -----------------------------------------------------------------------------
//
// This work is licensed under the Creative Commons Attribution-ShareAlike 4.0
// International License. To view a copy of this license, visit
//
// http://creativecommons.org/licenses/by-sa/4.0/
//
// or send a letter to Creative Commons,
// PO Box 1866, Mountain View, CA 94042, USA.
//
//******************************************************************************
#ifndef $header_name
#define $header_name

#include <Arduino.h>
#include <avr/pgmspace.h>
#include \"time.h\"
" > $output

echo "#define MAX_TIMEZONE_ID     ${#zone[@]}" >> $output
echo "#define TZ_DB_VERSION       \"$tzdb_version\"" >> $output
echo >> $output
echo >> $output

echo "const char TZ_UTC[] PROGMEM = { \"UTC\" };" >> $output
cat $filename_abbrev_list >> $output
echo >> $output
echo >> $output

echo "const char TZ_ETC_UTC[] PROGMEM = { \"Etc/UTC\" };" >> $output
cat $filename_tz_names >> $output
echo >> $output
echo >> $output

echo "const TimeZoneRules TimeZonesTable[] PROGMEM = {" >> $output
echo "    { TZ_UTC, 0, 0, 0, 0, 0, 0, TZ_UTC, 0, 0, 0, 0, 0, 0, TZ_UTC }," >> $output
cat $filename_tz_table >> $output
echo "};" >> $output

echo >> $output
echo "#endif /* $header_name */" >> $output

echo
echo "Done!"