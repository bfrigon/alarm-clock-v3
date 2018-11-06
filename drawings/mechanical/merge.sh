#!/bin/sh

TMP_METADATA_FILE="/tmp/_merge_sh_meta.dat"
TMP_PDF_OUTPUT="/tmp/_merge_sh_output.pdf"


change_drawing_title () {

    git diff-index --quiet --exit-code HEAD $1.pdf

    #### Check if the file was modified ####
    if [ $? -eq 1 ]; then

        echo "InfoBegin" > $TMP_METADATA_FILE
        echo "InfoKey: Title" >> $TMP_METADATA_FILE
        echo "InfoValue: $2" >> $TMP_METADATA_FILE
        echo "InfoBegin" >> $TMP_METADATA_FILE
        echo "InfoKey: Author" >> $TMP_METADATA_FILE
        echo "InfoValue: Benoit Frigon" >> $TMP_METADATA_FILE

        pdftk $1.pdf update_info $TMP_METADATA_FILE output $TMP_PDF_OUTPUT
        cp $TMP_PDF_OUTPUT $1.pdf
    fi
}

combine_drawing_files () {

    input_files=""
    current_page=1

    echo > $TMP_METADATA_FILE

    for drawing_id in $(echo $2 | tr "," "\n")
    do
        #### Dump metainfo from pdf ####
        meta=$(pdftk $1-$drawing_id*.pdf dump_data)

        #### Extract title and number of pages info ####
        title=$(echo "$meta" | sed -n '/InfoKey/ {  N ; s/InfoKey: Title\nInfoValue: \(.*\)/\1/p }')
        num_pages=$(echo "$meta" | sed -n 's/NumberOfPages: \(.*\)/\1/p')

        #### Add current drawing to the bookmark data ####
        echo "BookmarkBegin" >> $TMP_METADATA_FILE
        echo "BookmarkTitle: $title" >> $TMP_METADATA_FILE
        echo "BookmarkLevel: 1" >> $TMP_METADATA_FILE
        echo "BookmarkPageNumber: $current_page" >> $TMP_METADATA_FILE

        input_files="$input_files $1-$drawing_id*.pdf"

        current_page=$(($current_page+$num_pages))
    done


    pdftk $input_files cat output $TMP_PDF_OUTPUT
    pdftk $TMP_PDF_OUTPUT update_info $TMP_METADATA_FILE output $1.pdf
}



####################################################
echo "Changing title metadata in pdf files..."

change_drawing_title "clkv3-dwg-01_rgb-digits-pcb" "Display - RGB digit PCB"
change_drawing_title "clkv3-dwg-02_display-frame-front" "Display - Frame - Front"
change_drawing_title "clkv3-dwg-03_display-front-cover" "Display - Front cover"
change_drawing_title "clkv3-dwg-04_display-frame-back" "Display - Frame - Back"
change_drawing_title "clkv3-dwg-05_display-back-cover" "Display - Back cover"
change_drawing_title "clkv3-dwg-06_rgb-dots-pcb" "Display - RGB dots PCB"
change_drawing_title "clkv3-dwg-07_touch-keypad-pcb" "Touch keypad PCB"
change_drawing_title "clkv3-dwg-08_touch-keypad-overlay" "Touch keypad overlay"
change_drawing_title "clkv3-dwg-09_rgb-lamp-pcb" "RGB lamp PCB"
change_drawing_title "clkv3-dwg-10_enclosure-lower-section" "Enclosure - Lower section"
change_drawing_title "clkv3-dwg-11_enclosure-upper-section" "Enclosure - Upper section"
change_drawing_title "clkv3-dwg-12_front-panel-pcb" "Front panel PCB"
change_drawing_title "clkv3-dwg-13_enclosure-back-panel" "Enclosure - Back panel"
change_drawing_title "clkv3-dwg-14_motherboard-pcb" "Motherboard PCB"
change_drawing_title "clkv3-dwg-15_daughterboard-pcb" "Daughterboard PCB"
change_drawing_title "clkv3-dwg-16_psu-pcb" "PSU PCB"
change_drawing_title "clkv3-dwg-17_alarm-switch-pcb" "Alarm switch PCB"
change_drawing_title "clkv3-dwg-18_rtc-battery-door" "RTC Battery door"
change_drawing_title "clkv3-dwg-19_rgb-lamp-window-frame" "RGB lamp window - Frame"
change_drawing_title "clkv3-dwg-20_rgb-lamp-window-cover" "RGB lamp window - Cover"
change_drawing_title "clkv3-dwg-21_front-panel-window" "Front panel window"
change_drawing_title "clkv3-dwg-30_assy-display-frame-front" "ASSY - Display frame - Front"
change_drawing_title "clkv3-dwg-31_assy-display-frame-back" "ASSY - Display frame - Back"
change_drawing_title "clkv3-dwg-32_assy-display" "ASSY - Display panel"
change_drawing_title "clkv3-dwg-33_assy-motherboard" "ASSY - Motherboard PCB"
change_drawing_title "clkv3-dwg-34_assy-touch-keypad" "ASSY - Touch keypad"
change_drawing_title "clkv3-dwg-35_final-assembly" "Final assembly"
change_drawing_title "clkv3-dwg-50_dev-jig" "Jig - dev."
change_drawing_title "clkv3-dwg-51_motherboard-assy-jig" "Motherboard assembly jig"


####################################################
echo "Combining pdf files..."

combine_drawing_files "clkv3-dwg" "01,06,02,03,07,08,34,30,04,05,09,19,20,31,32,10,11,18,13,21,17,14,15,16,12,33,35"
change_drawing_title "clkv3-dwg" "Alarm clock V3"


echo
echo "Done"
