#!/bin/sh

#if the user has not entered --lower or --upper print help
if [[ "$1" != "--lower" && "$1" != "--upper" ]]
    then
    echo "\"./rename.sh --lower <directory>\" Changes file names to lowercase with underscores"
    echo "\"./rename.sh --upper <directory>\" Changes file names to upercase with spaces"
    echo ""
    echo "Replace <directory> with the path where you want file names changed."
    echo "Current directory is the default."
    exit 1
fi

#Find out if the user entered a directory
#if not then set it to the current directory
if [ -z "$2" ]
    then
    DIRECTORY=`pwd`"/"
else
    DIRECTORY="$2"
fi

#if the directory the user entered doesn't have a trailing / then add it
if [ "${DIRECTORY:${#DIRECOTRY}:1}" != "/" ]
    then
    DIRECTORY="$DIRECTORY""/"
fi

echo "Changing files names in $DIRECTORY"

# Move through every file in the directory and rename accordingly
for FILE in $DIRECTORY*
  do
  FILE="${FILE#./*}"
  FILE="${FILE##*/}"
  if [ "$FILE" != "rename.sh" ]
      then
      #Convert to lowercase with underscores
      if [ "$1" == "--lower" ]
    then
    #Convert file name to all lower case
    NEW_NAME=`echo $FILE | tr [:upper:] [:lower:]`
    #Convert spaces in file name to underscores
    NEW_NAME=`echo $NEW_NAME | tr [:blank:] "_"`
    if [ "$FILE" != "$NEW_NAME" ]
         then
         mv -v "$DIRECTORY""$FILE" "$DIRECTORY""$NEW_NAME"
    fi
    #Convert underscores to spaces and capitalize first letter of each word
    else
    #Convert underscores to spaces
    NEW_NAME=`echo $FILE | tr "_" " "`

    #Take apart each word and make the first letter upper case
    for i in $NEW_NAME
       do
       B=`echo -n "${i:0:1}" | tr "[:lower:]" "[:upper:]"`
       FINAL_NAME="$FINAL_NAME"`echo -n "${B}${i:1}"`
    done
    if [ "$FILE" != "$FINAL_NAME" ]
         then
         mv -v "$DIRECTORY""$FILE" "$DIRECTORY""$FINAL_NAME"
    fi
    #Clear final name because each word is appended to it
    FINAL_NAME=""
    fi
  fi
done
