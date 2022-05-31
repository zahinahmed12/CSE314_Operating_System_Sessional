#!/bin/bash

#if [ $# not in ls ]; then
	#echo "input your file name"
#fi

if [ $# -lt 1 ]; then
	echo "please run the script as: $0 input_file_name"
	exit
fi 


if [ ! -f "$1" ]
then
    echo "$1 not found!"
    exit 1
fi

input=$1
n_dir='.'

#mkdir ../output_dir
rm -rf ../output_dir
mkdir ../output_dir

#>output.csv
output_dir_path="$(realpath ../output_dir)"
current_dir=$(pwd)

>"$output_dir_path"/output.csv

printf "File_Path\tLine_no\tLine_containing_searched_string\n" >> "$output_dir_path"/output.csv


#echo $current_dir
#echo $output_dir_path

root="$(realpath .)"

#echo $root

base_root="${root##*/}"
remove_from_root="${root%/*$base_root}"

#echo $base_root $remove_from_root

declare -i one=1
declare -i number_of_files=0

b=$(head -n 1 "$input" )
nol=$(head -n 2 "$input" | tail -n 1)
word_to_search=$(head -n 3 "$input" | tail -n 1)

#echo $b

if [ "$b" = "begin" ]; then

	cmd="head"

elif [ "$b" = "end" ]; then

	cmd="tail"
fi

#if [ "$b"="end" ]; then

	#cmd="tail"
#fi



#echo "$cmd"
#echo "$nol"
#echo "$word_to_search"

#declare -a test_array
traverse_directories() {
    cd "$1"

	dot="."
	concat=""
	

	for f in *
	do
		#root="$(realpath .)"
		#base_root="${root##*/}"
		#concat=${base_root}${dot}${concat}
		#echo $base_root

		if [ -d "$f" ]; then
			# next level directory

			#root="$(realpath .)"
		        #base_root="${root##*/}"
			#echo $base_root
			#test_array+=($base_root)

			traverse_directories "$f"
			
			#concat=${base_root}${dot}${concat}
			
			
		elif [ -f "$f" ]; then
			if file "$f" | grep -q text ; then
				# text file; need to process
				if $cmd -n $nol $f | grep -qi $word_to_search; then
					fn="${f%.*}"
					ext="${f##*.}"


					if [ "$fn" = "$ext" ]; then

						ext=""
					else 
						ext=".$ext"
					fi

					number_of_files="(($number_of_files+$one))"
#echo "$cmd"

					
#echo "$cmd"
					if [ "$cmd" = "head" ]; then
						#echo "$cmd"
						line_no=$($cmd -n $nol $f | grep -ni $word_to_search | cut -f1 -d':' | $cmd -n 1)


					elif [ "$cmd" = "tail" ]; then
						#echo "$cmd"
						#$cmd -n $nol $f | grep -ni $word_to_search
						
						line_no=$("$cmd" -n "$nol" "$f" | grep -ni "$word_to_search" | cut -f1 -d':' | $cmd -n 1)
						line_total=$(< "$f" wc -l)
						#echo $line

						if [ $line_total -gt  $nol ]; then
						
							differ="$(($nol-$line_no))"
							line_no="$(($line_total-$differ))"
						fi
						
					
					fi

					b=$($cmd -n $nol $f | grep -ni $word_to_search | cut -f1 -d':')
					c=$($cmd -n $nol $f | grep -ni $word_to_search | cut -f2 -d':')

					#echo $concat
					#echo $number_of_files
					current_dir=$(pwd)
					find_root="${current_dir##/}"

					#a_root={${current_dir##}"$1"}

					#str="${current_dir////.}"
					str="${find_root////.}"
					new_file_name="${str}${dot}${fn}${line_no}${ext}"
            

                    			new_file_name="${base_start_path}${new_file_name#$start_path}"

                    			#echo $new_file_name
					
					#output_dir_path="$(realpath ../output_dir)"
					cp $f "${output_dir_path}/${new_file_name}"


					#echo "$new_file_name,$b,$c" >> "output.csv" 

					#printf "$new_file_name\thi\t${b}\n" >> "$output_dir_path"/output.csv


					#all_the_lines=$("$cmd" -n "$nol" "$f" | grep -ni "$word_to_search" | cut -f1 -d':')


					lines=`${cmd} -${nol} $f | grep -n ${word_to_search} | sed 's/:/	/'`
			
						IFS=$'\n'

						for line in $lines
						do

							file_name="${str}${dot}${fn}${ext}"
            
                    					file_name="${base_start_path}${file_name#$start_path}"

							printf "${file_name}\t${line}\n" >> "$output_dir_path"/output.csv
						done

				fi
			else
				echo ""
				#echo $f is a non text file
			fi
		fi
	done

	cd ../
}

cd "$2"
start_path=$(pwd)
start_path=${start_path////.}
start_path=${start_path:1}
base_start_path=${start_path##*.}

traverse_directories . 

echo "Total ${number_of_files} matched the criteria!"
