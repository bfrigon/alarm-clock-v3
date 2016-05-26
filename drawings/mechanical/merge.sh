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
        meta=$(pdftk clkv3-dwg-$drawing_id.pdf dump_data)
        
        #### Extract title and number of pages info ####
        title=$(echo "$meta" | sed -n '/InfoKey/ {  N ; s/InfoKey: Title\nInfoValue: \(.*\)/\1/p }')
        num_pages=$(echo "$meta" | sed -n 's/NumberOfPages: \(.*\)/\1/p')
        
        #### Add current drawing to the bookmark data ####
        echo "BookmarkBegin" >> $TMP_METADATA_FILE
        echo "BookmarkTitle: $title" >> $TMP_METADATA_FILE
        echo "BookmarkLevel: 1" >> $TMP_METADATA_FILE
        echo "BookmarkPageNumber: $current_page" >> $TMP_METADATA_FILE
        
        input_files="$input_files clkv3-dwg-$drawing_id.pdf"

        current_page=$(($current_page+$num_pages))
    done
    
    
    pdftk $input_files cat output $TMP_PDF_OUTPUT
    pdftk $TMP_PDF_OUTPUT update_info $TMP_METADATA_FILE output $1.pdf
}



####################################################
echo "Changing title metadata in pdf files..."

change_drawing_title "clkv3-dwg-01" "Display - RGB digit PCB" 
change_drawing_title "clkv3-dwg-02" "Display - Frame" 
change_drawing_title "clkv3-dwg-03" "Display - Back cover" 
change_drawing_title "clkv3-dwg-04" "Display - Front cover" 
change_drawing_title "clkv3-dwg-05" "Display - RGB dots PCB"
change_drawing_title "clkv3-dwg-06" "Touch keypad PCB" 
change_drawing_title "clkv3-dwg-07" "Touch keypad overlay"
change_drawing_title "clkv3-dwg-20" "ASSY - Display panel" 

####################################################
echo "Combining pdf files for model V3..."

combine_drawing_files "clkv3-dwg" "01,02,03,04,05,06,07,20"
change_drawing_title "clkv3-dwg" "Alarm clock V3"


echo        
echo "Done"
