/*(c) 2006 Christopher James Meacham (alias Optimus[ Prime])
*
*Usage of the works is permitted provided that this instrument is retained with the works, so that any entity that uses the works is notified of this instrument.
*
*DISCLAIMER: THE WORKS ARE WITHOUT WARRANTY.
*
*[2004, Fair License: rhid.com/fair]
*/

#include <time.h>
#include <stdio.h>
#include <stdlib.h> //Standard C Strings
#include <iostream>
#include <fstream>
#include <dirent.h> //Directory scanning
#include <errno.h>

//There are no symbolic links in Windows so ELOOP will be undefined
#ifndef ELOOP
const int ELOOP=0;
#else
#define _UNIX_
#endif

int verbosity=0;

using namespace std;

//Encode for URL
void encode_url(string& output,
                string input)
{
    int pos;
    string temp=input, replace_output, replace_temp;
    unsigned char i;
    char j[3]="";

    output = input;

    i = 0x25;
    while (temp.find(i) != string::npos)
    {
        //find space
        pos = temp.find(i);
        //and replace
        sprintf(j, "%02X", i);
        replace_temp = "0";
        replace_temp += j;
        temp.replace(pos, 1, replace_temp);
        replace_output = "%";
        replace_output += j;
        output.replace(pos, 1, replace_output);
    }
    for (i = 0x20; i < 0x25; i++)
    {
        while (temp.find(i) != string::npos)
        {
            //find space
            pos = temp.find(i);
            //and replace
            sprintf(j, "%02X", i);
            replace_temp = "0";
            replace_temp += j;
            temp.replace(pos, 1, replace_temp);
            replace_output = "%";
            replace_output += j;
            output.replace(pos, 1, replace_output);
        }
    }
    //Linux Hack
    while (temp.find(0x26) != string::npos)
    {
        //find &
        pos = temp.find(0x26);
        //and replace
        char replace_len = 1;
        if (temp.substr(pos, 6) == "&quot;")
        {
            sprintf(j, "22");
            replace_len = 6;
        }
        else if (temp.substr(pos, 5) == "&amp;")
        {
            sprintf(j, "26");
            replace_len = 5;
        }
        else if (temp.substr(pos, 6) == "&apos;")
        {
            sprintf(j, "27");
            replace_len = 6;
        }
        else if (temp.substr(pos, 7) == "&frasl;")
        {
            sprintf(j, "2F");
            replace_len = 7;
        }
        else if (temp.substr(pos, 4) == "&lt;")
        {
            sprintf(j, "3C");
            replace_len = 4;
        }
        else if (temp.substr(pos, 4) == "&gt;")
        {
            sprintf(j, "3E");
            replace_len = 4;
        }
        else if (temp.substr(pos, 6) == "&nbsp;")
        {
            sprintf(j, "20");
            replace_len = 6;
        }
        else
            sprintf(j, "26");
        replace_temp = "0";
        replace_temp += j;
        temp.replace(pos, replace_len, replace_temp);
        replace_output = "%";
        replace_output += j;
        output.replace(pos, replace_len, replace_output);
    }
    for (i = 0x27; i < 0x2D; i++)
    {
        while (temp.find(i) != string::npos)
        {
            //find space
            pos = temp.find(i);
            //and replace
            sprintf(j, "%02X", i);
            replace_temp = "0";
            replace_temp += j;
            temp.replace(pos, 1, replace_temp);
            replace_output = "%";
            replace_output += j;
            output.replace(pos, 1, replace_output);
        }
    }
    for (i = 0x3B; i < 0x41; i++)
    {
        while (temp.find(i) != string::npos)
        {
            //find space
            pos = temp.find(i);
            //and replace
            sprintf(j, "%02X", i);
            replace_temp = "0";
            replace_temp += j;
            temp.replace(pos, 1, replace_temp);
            replace_output = "%";
            replace_output += j;
            output.replace(pos, 1, replace_output);
        }
    }
    for (i = 0x5B; i < 0x61; i++)
    {
        while (temp.find(i) != string::npos)
        {
            //find space
            pos = temp.find(i);
            //and replace
            sprintf(j, "%02X", i);
            replace_temp = "0";
            replace_temp += j;
            temp.replace(pos, 1, replace_temp);
            replace_output = "%";
            replace_output += j;
            output.replace(pos, 1, replace_output);
        }
    }
    for (i = 0x7B; i <= 0x7F; i++)
    {
        while (temp.find(i) != string::npos)
        {
            //find space
            pos = temp.find(i);
            //and replace
            sprintf(j, "%02X", i);
            replace_temp = "0";
            replace_temp += j;
            temp.replace(pos, 1, replace_temp);
            replace_output = "%";
            replace_output += j;
            output.replace(pos, 1, replace_output);
        }
    }
}

//Check strings for special characters
void replace_special(   string& output,
                        string input)
{
    int pos;
    string temp=input;

    output = input;

    while (temp.find('&') != string::npos)
    {
        //find space
        pos = temp.find('&');
        //and replace
        temp.replace(pos, 1, "space");
        output.replace(pos, 1, "&amp;");
    }
    while (output.find('<') != string::npos)
    {
        //find space
        pos = output.find('<');
        //and replace
        output.replace(pos, 1, "&lt;");
    }
    while (output.find('>') != string::npos)
    {
        //find space
        pos = output.find('>');
        //and replace
        output.replace(pos, 1, "&gt;");
    }
    while (output.find('\'') != string::npos)
    {
        //find space
        pos = output.find('\'');
        //and replace
        output.replace(pos, 1, "&apos;");
    }
    while (output.find('\"') != string::npos)
    {
        //find space
        pos = output.find('\"');
        //and replace
        output.replace(pos, 1, "&quot;");
    }
}

//convert string from mixed case to lowercase
void string_tolower( string& str)
{
    int length=str.length();
    for (int i=0; i < length; i++)
    {
        if (str[i] > 'A' && str[i] < 'Z')
            str[i] -= ('A' - 'a');
    }
}

//Search local directory for audio files
int get_audiolist( string directory,
                    string& audiolist,
                    int major,
                    int minor)
{
    DIR *directory_pointer;
    dirent *file;
    int length=0;
    char error[3]="0";

    //Clear audiolist
    audiolist = "";

    //Set Directory
    directory_pointer = opendir(directory.c_str());

    //Catch error
    if (directory_pointer == NULL)
    {
        cout << "\nError accessing the directory:\n\tError #";
        sprintf(error, "%d", errno);
        cout << error;
        cout << " = ";
        switch (errno)
        {
            case EACCES:        cout << "Search permission is denied for a component of the path\n";
                                cout << "\t\tprefix of directory or read permission is denied for directory.\n";
                                break;
            case ELOOP:         cout << "Too many symbolic links were encountered in resolving the\n";
                                cout << "\t\tpath.\n";
                                break;
            case ENAMETOOLONG:  cout << "The length of the directory name exceeds {PATH_MAX}, a\n";
                                cout << "\t\tpathname component is longer than {NAME_MAX}, or pathname resolution\n";
                                cout << "\t\tof a symbolic link produced an intermediate result whose length\n";
                                cout << "\t\texceeds {PATH_MAX}.\n";
                                break;
            case ENOENT:        cout << "A component of the path does not name an existing directory\n";
                                cout << "\t\tor a directory name was not given.\n";
                                break;
            case ENOTDIR:       cout << "A component of the path is not a directory.\n";
                                break;
            case EMFILE:        cout << "The maximum number of file descriptors are currently open in\n";
                                cout << "\t\tthe calling process.\n";
                                break;
            case ENFILE:        cout << "Too many files are currently open in the system.\n";
                                break;
            default:            cout << "Unknown Error\n";
                                break;
        }
	cout << "Couldn't open directory " << directory << " to search for image file. Giving up.\n";
	return 1;
    }

    //Compose audio file list
    while ((file = readdir(directory_pointer)) != NULL)
    {
        //Check file extension
        string temp = file->d_name, temp2;

        length = temp.length();
        if (length > 4)
        {
            temp2 = temp.substr(temp.rfind('.'), length - temp.rfind('.'));
            if (major > 2 || (major == 2 && minor >= 80))
            {
                if (temp2 == ".mp3" ||
                    temp2 == ".m4a" ||
                    temp2 == ".mp4" ||
                    temp2 == ".m4v" ||
                    temp2 == ".jpeg" ||
                    temp2 == ".jpg" ||
                    temp2 == ".gif" ||
                    temp2 == ".png" ||
                    temp2 == ".tiff" ||
                    temp2 == ".tif" ||
                    temp2 == ".bmp")
                {
                    audiolist += temp;
                    audiolist += "/";
                }
            }
            else if (major == 2 && (minor < 80 && minor >= 70))
            {
                if (temp2 == ".mp3" ||
                    temp2 == ".m4a" ||
                    temp2 == ".mp4")
                {
                    audiolist += temp;
                    audiolist += "/";
                }
            }
            else if (major == 2 && minor < 70)
            {
                if (temp2 == ".mp3" ||
                    temp2 == ".mp4")
                {
                    audiolist += temp;
                    audiolist += "/";
                }
            }
        }
    }
    closedir(directory_pointer);

    //Debug
    if (verbosity >= 2)
        printf("audiolist ==>%s\n", audiolist.c_str());

    return 0;
}

//Search local directory for image files and take the first one found
int get_image( string directory,
                string& image)
{
    DIR *directory_pointer;
    dirent *file;
    fstream imagefile;
    int length=0;
    streampos size=0;
    char error[3]="0";

    //Set Directory
    directory_pointer = opendir(directory.c_str());

    //Catch error
    if (directory_pointer == NULL)
    {
        cout << "\nError accessing the directory:\n\tError #";
        sprintf(error, "%d", errno);
        cout << error;
        cout << " = ";
        switch (errno)
        {
            case EACCES:        cout << "Search permission is denied for a component of the path\n";
                                cout << "\t\tprefix of directory or read permission is denied for directory.\n";
                                break;
            case ELOOP:         cout << "Too many symbolic links were encountered in resolving the\n";
                                cout << "\t\tpath.\n";
                                break;
            case ENAMETOOLONG:  cout << "The length of the directory name exceeds {PATH_MAX}, a\n";
                                cout << "\t\tpathname component is longer than {NAME_MAX}, or pathname resolution\n";
                                cout << "\t\tof a symbolic link produced an intermediate result whose length\n";
                                cout << "\t\texceeds {PATH_MAX}.\n";
                                break;
            case ENOENT:        cout << "A component of the path does not name an existing directory\n";
                                cout << "\t\tor a directory name was not given.\n";
                                break;
            case ENOTDIR:       cout << "A component of the path is not a directory.\n";
                                break;
            case EMFILE:        cout << "The maximum number of file descriptors are currently open in\n";
                                cout << "\t\tthe calling process.\n";
                                break;
            case ENFILE:        cout << "Too many files are currently open in the system.\n";
                                break;
            default:            cout << "Unknown Error\n";
                                break;
        }
	cout << "Couldn't open directory " << directory << " to search for image file. Giving up on image file.\n";
	return 1;
    }

    //Get image filename
    while ((file = readdir(directory_pointer)) != NULL)
    {
        //Check file extension
        string temp = file->d_name, temp2;

        length = temp.length();
        if (length > 4)
        {
            temp2 = temp.substr(temp.rfind('.'), length - temp.rfind('.'));
            if (temp2 == ".gif" ||
                temp2 == ".jpg" ||
                temp2 == ".png" ||
                temp2 == ".bmp" ||
                temp2 == ".tif" ||
                temp2 == ".tiff")
            {
                //Check that image file is small enough
                if (directory[0] == '/')
                    imagefile.open((directory + "/" + temp).c_str(), fstream::in | ios_base::binary);
                else
                    imagefile.open((directory + "\\" + temp).c_str(), fstream::in | ios_base::binary);
                imagefile.seekg(0, ios_base::end);
                size = imagefile.tellg();
                if (size > 204800)
                    continue;

                //Set image and exit
                image = temp;
                closedir(directory_pointer);
                return 0;
            }
        }
    }
    closedir(directory_pointer);
    return 0;
}

int create_xml_file(fstream& xmlfile,
                    string xmlfilename,
                    string directory,
                    string image,
                    string title,
                    string link,
                    string description,
                    string copyright)
{
    FILE *file;
    string temp, image_link="";
    char temp2[1024];

    if (image != "")
    {
        //Check file strings for spaces
        encode_url(temp, directory);
        image_link = temp;
        image_link += '/';
        encode_url(temp, image);
        image_link += temp;
        temp="";
    }

    //Create xml file and setup XML header.
    file = fopen(xmlfilename.c_str(), "a");
    fclose(file);
    xmlfile.open(xmlfilename.c_str(), fstream::in|fstream::out);
    if (!xmlfile.is_open())
    {
        cout << "Error opening xml file:" << xmlfilename << ".\n";
        return 1;
    }

    //If file already exists, seek past header.
    xmlfile.peek();
    if (xmlfile.eof())
    {
        //Clear eof bit
        xmlfile.clear();

        //create header
        xmlfile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        xmlfile << "<rss version=\"2.0\">\n";
        xmlfile << " <channel>\n";
        xmlfile << "  <title>" << title << "</title>\n";
        if (link != "")
        {
            xmlfile << "  <link>" << link << "</link>\n";
        }
        if (description != "")
        {
            xmlfile << "  <description>" << description << "</description>\n";
        }
        xmlfile << "  <language>en-us</language>\n";
        if (copyright != "")
        {
            xmlfile << "  <copyright>" << copyright << "</copyright>\n";
        }

        if (image_link != "")
        {
            xmlfile << "  <image>\n";
            xmlfile << "   <url>" << image_link << "</url>\n";
            xmlfile << "   <title>" << title << "</title>\n";
            xmlfile << "  </image>\n";
        }

    }
    else
    {
        //seek past header and any previously created channels
        while (1)
        {
            xmlfile.getline(temp2, 1024);
            temp = temp2;
            if (temp.find("</channel>") != string::npos)
            {
                xmlfile.getline(temp2, 1024);
                temp = temp2;
                if (temp.find("<channel>") == string::npos)
                {
                    int place, count;

                    //Clear error flags
                    if (xmlfile.eof())
                        xmlfile.clear();

                    //Do the math
                    place=xmlfile.tellg();
                    count=xmlfile.gcount();
                    xmlfile.seekg(place-count);
                    xmlfile.seekp(place-count);

                    //Debug
                    if (verbosity >= 4)
                    printf("place=%d\ncount=%d\nxmlfile.tellg()=%d\nxmlfile.tellp()=%d\n", place, count, (int)xmlfile.tellg(), (int)xmlfile.tellp());
                    break;
                }
            }
            if (xmlfile.eof())
            {
                cout << "Error in original XML file. No channel close tag (i.e., \"</channel>\").\n";
                return 1;
            }
        }

        //Add channel header
        xmlfile << " <channel>\n";
        xmlfile << "  <title>" << title << "</title>\n";
        if (link != "")
        {
            xmlfile << "  <link>" << link << "</link>\n";
        }
        if (description != "")
        {
            xmlfile << "  <description>" << description << "</description>\n";
        }
        xmlfile << "  <language>en-us</language>\n";
        if (copyright != "")
        {
            xmlfile << "  <copyright>" << copyright << "</copyright>\n";
        }

        if (image_link != "")
        {
            xmlfile << "  <image>\n";
            xmlfile << "   <url>" << image_link << "</url>\n";
            xmlfile << "   <title>" << title << "</title>\n";
            xmlfile << "  </image>\n";
        }
    }

    return 0;
}

//length is equal to the total number of characters in the buffer
//(i.e., 0-1023 signifies 1024 characters)
void remove_nulls(char text[], int length)
{
    int modified_length=length, i, j;

    for (i=0; i<modified_length; i++)
    {
        //If current character is null, remove
        if (text[i] == 0)
        {
            modified_length--;
            for (j=i+1; j<length; j++)
                text[j-1] = text[j];
            text[length-1]=0;
        }
    }
}

void order_movielist(string& audiolist, string movie_order)
{
    string audio="", picture="", video="", file, extension;
    int length=0;

    //check for non sorted values
    if (movie_order == "none" || movie_order == "file")
        return;

    while(audiolist.length() > 6)
    {
        //Get audio file name
        length = audiolist.find('/');
        file = audiolist.substr(0, length);

        //Remove file from list
        audiolist.replace(0, length+1, "");

        //Check for compatibility
        extension = file.substr(file.rfind('.'), length - file.rfind('.'));

        if (extension == ".mp3" ||
            extension == ".m4a")
        {
            audio += (file + "/");
        }
        else if (   extension == ".mp4" ||
                    extension == ".m4v")
        {
            video += (file + "/");
        }
        else if (   extension == ".jpeg" ||
                    extension == ".jpg" ||
                    extension == ".gif" ||
                    extension == ".png" ||
                    extension == ".tiff" ||
                    extension == ".tif" ||
                    extension == ".bmp")
        {
            if (movie_order == "channel")
                video += (file + "/");
            else
                picture += (file + "/");
        }
    }
    audiolist = audio + picture + video;
}

//input
//=======================
//fstream& audiofile
//      previously opened mp3 file
//-----------------------
//output
//=======================
//int& version
//      the id3 versions supported in this mp3 file
//      0x1     ID3v1
//      0x2     ID3v2
//      0x3     ID3v1 and ID3v2
void check_ID3_version(fstream& mp3file, int& version)
{
    char temp[8];

    //Check for ID3v2 tag
    mp3file.seekg(0);
    mp3file.readsome(temp, 3);
    temp[3] = 0;

    //Check for ID3v1 tag
    mp3file.seekg(127, ios_base::end);
    mp3file.readsome(temp+4, 3);
    temp[7] = 0;

    //Debug
    if (verbosity >= 2)
        printf("ID3v2 = %s\nID3v1 = %s\n", temp, temp+4);

    //Set output
    version = 0;
    if (strcmp(temp, "ID3") == 0)
        version += 2;
    if (strcmp(temp+4, "TAG") == 0)
        version += 1;
}

//This function does not extract the MCDI or PRIV tags.
//It may be missing other tags as well. I only implemented
//tags necessary for Music and Video feeds.
//
//input
//=======================
//fstream& audiofile
//      previously opened mp3 file
//int max_tags
//      max number of tags to be extracted
//-----------------------
//output
//=======================
//string* Tags
//      storage for ID3 tag names
//string* Values
//      storage for ID3 tag values
void extract_ID3v2_tags(fstream& mp3file, string *Tags, string *Values, int max_tags)
{
    char temp[1024];
    string temp2;
    int id3_length=0, frame_length=0, i=0, cursor=0, tag_number = 0;

    //Get id3 length
    //ID3 Header is 3 characters (ID3),
    //2 bytes for version number,
    //1 byte for flags,
    //and 4 bytes for size
    //  (Size is 28-bit integer composed of
    //   the last 7 bits of each byte, most
    //   significant first. It does not
    //   include the 10 byte ID3 header.)
    mp3file.seekg(cursor);
    mp3file.readsome(temp, 10);
    cursor += 10;
    i=6;
    id3_length = 10;
    //Most Significant byte * 128^3
    id3_length += temp[i++]*2097152;
    //Next to Most Significant byte * 128^2
    id3_length += temp[i++]*16384;
    //Next Least Significant byte * 128
    id3_length += temp[i++]*128;
    //Least Significant byte
    id3_length += temp[i++];
    temp[0] = 0;

    //Debug
    if (verbosity >= 1)
        printf("id3_length = %X\n", id3_length);

    //Parse file info
    while (cursor < id3_length && tag_number < max_tags)
    {
        //Frame Header is four characters,
        //four bytes for size
        //  (Size is 28-bit integer composed of the
        //   last 7 bits of each byte, most significant
        //   first [except in the case of the MCDI frame].
        //   It does not include the 10 byte frame header.),
        //2 bytes for flags,
        //and 1 byte for text type
        //  (0x00 = ISO-8859-1,
        //   0x01 = UTF-16,
        //   0x02 = UTF-16BE,
        //   0x03 = UTF-8)
        mp3file.seekg(cursor);

        //Debug
        if (verbosity >= 4)
            printf("pointer = %X\ncursor = %X\n", (int)mp3file.tellg(), cursor);

        mp3file.readsome(temp, 10);
        cursor += 10;
        i=4;

        //Padding at end of ID3 tag (supposedly version 4 and up only, but it is in version 3 sometimes)
        if (temp[0]==0)
            break;
        //The MCDI frame size is a direct 32-bit integer
        else if (temp[0]=='M' && temp[1]=='C' && temp[2]=='D' && temp[3]=='I')
        {
            //Most Significant byte * 256^3
            frame_length = (temp[i++] & 0xFF)*16777216;
            //Next to Most Significant byte * 256^2
            frame_length += (temp[i++] & 0xFF)*65536;
            //Next Least Significant byte * 256
            frame_length += (temp[i++] & 0xFF)*256;
            //Least Significant byte
            frame_length += (temp[i++] & 0xFF);
            cursor += frame_length;
            frame_length=0;
            continue;
        }
        else
        {
            //Most Significant byte * 128^3
            frame_length = (temp[i++] & 0xFF)*2097152;
            //Next to Most Significant byte * 128^2
            frame_length += (temp[i++] & 0xFF)*16384;
            //Next Least Significant byte * 128
            frame_length += (temp[i++] & 0xFF)*128;
            //Least Significant byte
            frame_length += (temp[i++] & 0xFF);
        }

        //Check for major screw up
        if (frame_length > (id3_length - cursor))
            break;

        //Copy Tag name
        temp[4]=0;
        Tags[tag_number] = temp;

        //Debug
        if (verbosity >= 4)
        {
            printf("temp = %s\nframe_length = %X, temp[5-9] = %02X%02X%02X%02X%02X\n", temp, frame_length, temp[5] & 0xFF, temp[6] & 0xFF, temp[7] & 0xFF, temp[8] & 0xFF, temp[9] & 0xFF);
            printf("Tags[%i]=%s\n", tag_number, Tags[tag_number].c_str());
        }

        if (Tags[tag_number] == "PRIV")
        {
            cursor += frame_length;
            frame_length=0;
            continue;
        }

        //Read character format
        mp3file.seekg(cursor);
        mp3file.readsome(temp, 1);
        cursor += 1;
        frame_length--;

        //If UTF-16
        if (temp[0] == 0x01 || temp[0] == 0x02)
        {
            //First 2 bytes appear to be unimportant for ASCII values
            cursor += 2;
	    frame_length -= 2;
 
            while (frame_length > 0)
            {
                //If less than 1024 bytes left read difference
                if (frame_length < 1024)
                {
                    mp3file.seekg(cursor);
                    mp3file.readsome(temp, frame_length);
                    cursor += frame_length;
                    remove_nulls(temp, frame_length);
                    frame_length=0;
                }
                else
                {
                    mp3file.seekg(cursor);
                    mp3file.readsome(temp, 1024);
                    cursor += 1024;
                    remove_nulls(temp, 1024);
                    frame_length -= 1024;
                }
                Values[tag_number] += temp;

                //Debug
                if (verbosity >= 4)
                    printf("temp = %s\nValues[%i] = %s\n", temp, tag_number, Values[tag_number].c_str());

                temp[0]=0;
            }
        }
        //If ISO or UTF-8 or other
        else //if (temp[0] == 0x00 || temp[0] == 0x03)
        {
            while (frame_length > 0)
            {
                //If less than 1024 bytes left read difference
                if (frame_length < 1024)
                {
                    mp3file.seekg(cursor);
                    mp3file.readsome(temp, frame_length);
                    cursor += frame_length;
                    temp[frame_length]=0;
                    frame_length=0;
                }
                else
                {
                    mp3file.seekg(cursor);
                    mp3file.readsome(temp, 1024);
                    cursor += 1024;
                    frame_length -= 1024;
                }
                Values[tag_number] += temp;

                //Debug
                if (verbosity >= 4)
                    printf("temp = %s\nValues[%i] = %s\n", temp, tag_number, Values[tag_number].c_str());

                temp[0]=0;
            }
        }
        if (Tags[tag_number] == "TCON")
        {
            bool switch_check=false;

            //Set Content Type
            if (Values[tag_number][0] == '(' && Values[tag_number][1] == '(')
        {
                //remove double '('
                Values[tag_number].erase(0,1);
            }
            else if (Values[tag_number][0] == '(')
            {
                int right_side=0;

                //find the right parenthesis
                right_side = Values[tag_number].find(')');
                if (Values[tag_number][right_side + 1] == 0)
                {
                    if (right_side > 4)
                    {
                        Values[tag_number].erase(right_side,1);
                        Values[tag_number].erase(0,1);
                    }
                    else
                    {
                        for (int k=1; k < right_side; k++)
                        {
                            if (Values[tag_number][k] < '0' || Values[tag_number][k] > '9')
                            {
                                Values[tag_number].erase(right_side,1);
                                Values[tag_number].erase(0,1);
                                switch_check = false;
                            }
                            else
                            {
                                temp[k-1] = Values[tag_number][k];
                                switch_check = true;
                            }
                        }
                        if (switch_check)
                        {
                            temp[right_side - 1] = 0;
                            i = atoi(temp);

                            //Debug
                            if (verbosity >= 4)
                                printf("temp=%s\ni=%i", temp, i);
                        }
                    }
                }
                else
                {
                    if (Values[tag_number][right_side + 1] == '(' && Values[tag_number][right_side + 2] == '(')
                        Values[tag_number].erase(0,right_side + 2);
                    else
                        Values[tag_number].erase(0,right_side + 1);
                }
            }
            else if (Values[tag_number].length() <= 3)
            {
            int right_side=0;

                //check if TCON has nothing but a number
        right_side = Values[tag_number].length();
        for (int k=0; k < right_side; k++)
                {
                    if (Values[tag_number][k] < '0' || Values[tag_number][k] > '9')
                    {
                        switch_check = false;
                    }
                    else
                    {
                        temp[k] = Values[tag_number][k];
                        switch_check = true;
                    }
                }
                if (switch_check)
                {
                    temp[right_side] = 0;
                    i = atoi(temp);

                    //Debug
                    if (verbosity >= 4)
                        printf("temp=%s\ni=%i", temp, i);
                }
            }

            //Debug
            if (verbosity >= 4)
                printf("i = %i\nValues[%i] = %s\n", i, tag_number, Values[tag_number].c_str());

            if (switch_check)
            {
                switch (i)
                {
                    //Genre table
                    case 0:     Values[tag_number] = "Blues";
                                break;
                    case 1:     Values[tag_number] = "Classic Rock";
                                break;
                    case 2:     Values[tag_number] = "Country";
                                break;
                    case 3:     Values[tag_number] = "Dance";
                                break;
                    case 4:     Values[tag_number] = "Disco";
                                break;
                    case 5:     Values[tag_number] = "Funk";
                                break;
                    case 6:     Values[tag_number] = "Grunge";
                                break;
                    case 7:     Values[tag_number] = "Hip-Hop";
                                break;
                    case 8:     Values[tag_number] = "Jazz";
                                break;
                    case 9:     Values[tag_number] = "Metal";
                                break;
                    case 10:    Values[tag_number] = "New Age";
                                break;
                    case 11:    Values[tag_number] = "Oldies";
                                break;
                    case 12:    Values[tag_number] = "Other";
                                break;
                    case 13:    Values[tag_number] = "Pop";
                                break;
                    case 14:    Values[tag_number] = "R&B";
                                break;
                    case 15:    Values[tag_number] = "Rap";
                                break;
                    case 16:    Values[tag_number] = "Reggae";
                                break;
                    case 17:    Values[tag_number] = "Rock";
                                break;
                    case 18:    Values[tag_number] = "Techno";
                                break;
                    case 19:    Values[tag_number] = "Industrial";
                                break;
                    case 20:    Values[tag_number] = "Alternative";
                                break;
                    case 21:    Values[tag_number] = "Ska";
                                break;
                    case 22:    Values[tag_number] = "Death Metal";
                                break;
                    case 23:    Values[tag_number] = "Pranks";
                                break;
                    case 24:    Values[tag_number] = "Soundtrack";
                                break;
                    case 25:    Values[tag_number] = "Euro-Techno";
                                break;
                    case 26:    Values[tag_number] = "Ambient";
                                break;
                    case 27:    Values[tag_number] = "Trip-Hop";
                                break;
                    case 28:    Values[tag_number] = "Vocal";
                                break;
                    case 29:    Values[tag_number] = "Jazz+Funk";
                                break;
                    case 30:    Values[tag_number] = "Fusion";
                                break;
                    case 31:    Values[tag_number] = "Trance";
                                break;
                    case 32:    Values[tag_number] = "Classical";
                                break;
                    case 33:    Values[tag_number] = "Instrumental";
                                break;
                    case 34:    Values[tag_number] = "Acid";
                                break;
                    case 35:    Values[tag_number] = "House";
                                break;
                    case 36:    Values[tag_number] = "Game";
                                break;
                    case 37:    Values[tag_number] = "Sound Clip";
                                break;
                    case 38:    Values[tag_number] = "Gospel";
                                break;
                    case 39:    Values[tag_number] = "Noise";
                                break;
                    case 40:    Values[tag_number] = "AlternRock";
                                break;
                    case 41:    Values[tag_number] = "Bass";
                                break;
                    case 42:    Values[tag_number] = "Soul";
                                break;
                    case 43:    Values[tag_number] = "Punk";
                                break;
                    case 44:    Values[tag_number] = "Space";
                                break;
                    case 45:    Values[tag_number] = "Meditative";
                                break;
                    case 46:    Values[tag_number] = "Instrumental Pop";
                                break;
                    case 47:    Values[tag_number] = "Instrumental Rock";
                                break;
                    case 48:    Values[tag_number] = "Ethnic";
                                break;
                    case 49:    Values[tag_number] = "Gothic";
                                break;
                    case 50:    Values[tag_number] = "Darkwave";
                                break;
                    case 51:    Values[tag_number] = "Techno-Industrial";
                                break;
                    case 52:    Values[tag_number] = "Electronic";
                                break;
                    case 53:    Values[tag_number] = "Pop-Folk";
                                break;
                    case 54:    Values[tag_number] = "Eurodance";
                                break;
                    case 55:    Values[tag_number] = "Dream";
                                break;
                    case 56:    Values[tag_number] = "Southern Rock";
                                break;
                    case 57:    Values[tag_number] = "Comedy";
                                break;
                    case 58:    Values[tag_number] = "Cult";
                                break;
                    case 59:    Values[tag_number] = "Gangsta";
                                break;
                    case 60:    Values[tag_number] = "Top 40";
                                break;
                    case 61:    Values[tag_number] = "Christian Rap";
                                break;
                    case 62:    Values[tag_number] = "Pop/Funk";
                                break;
                    case 63:    Values[tag_number] = "Jungle";
                                break;
                    case 64:    Values[tag_number] = "Native American";
                                break;
                    case 65:    Values[tag_number] = "Cabaret";
                                break;
                    case 66:    Values[tag_number] = "New Wave";
                                break;
                    case 67:    Values[tag_number] = "Psychadelic";
                                break;
                    case 68:    Values[tag_number] = "Rave";
                                break;
                    case 69:    Values[tag_number] = "Showtunes";
                                break;
                    case 70:    Values[tag_number] = "Trailer";
                                break;
                    case 71:    Values[tag_number] = "Lo-Fi";
                                break;
                    case 72:    Values[tag_number] = "Tribal";
                                break;
                    case 73:    Values[tag_number] = "Acid Punk";
                                break;
                    case 74:    Values[tag_number] = "Acid Jazz";
                                break;
                    case 75:    Values[tag_number] = "Polka";
                                break;
                    case 76:    Values[tag_number] = "Retro";
                                break;
                    case 77:    Values[tag_number] = "Musical";
                                break;
                    case 78:    Values[tag_number] = "Rock & Roll";
                                break;
                    case 79:    Values[tag_number] = "Hard Rock";
                                break;
                    case 80:    Values[tag_number] = "Folk";
                                break;
                    case 81:    Values[tag_number] = "Folk-Rock";
                                break;
                    case 82:    Values[tag_number] = "National Folk";
                                break;
                    case 83:    Values[tag_number] = "Swing";
                                break;
                    case 84:    Values[tag_number] = "Fast Fusion";
                                break;
                    case 85:    Values[tag_number] = "Bebob";
                                break;
                    case 86:    Values[tag_number] = "Latin";
                                break;
                    case 87:    Values[tag_number] = "Revival";
                                break;
                    case 88:    Values[tag_number] = "Celtic";
                                break;
                    case 89:    Values[tag_number] = "Bluegrass";
                                break;
                    case 90:    Values[tag_number] = "Avantgarde";
                                break;
                    case 91:    Values[tag_number] = "Gothic Rock";
                                break;
                    case 92:    Values[tag_number] = "Progressive Rock";
                                break;
                    case 93:    Values[tag_number] = "Psychedelic Rock";
                                break;
                    case 94:    Values[tag_number] = "Symphonic Rock";
                                break;
                    case 95:    Values[tag_number] = "Slow Rock";
                                break;
                    case 96:    Values[tag_number] = "Big Band";
                                break;
                    case 97:    Values[tag_number] = "Chorus";
                                break;
                    case 98:    Values[tag_number] = "Easy Listening";
                                break;
                    case 99:    Values[tag_number] = "Acoustic";
                                break;
                    case 100:   Values[tag_number] = "Humour";
                                break;
                    case 101:   Values[tag_number] = "Speech";
                                break;
                    case 102:   Values[tag_number] = "Chanson";
                                break;
                    case 103:   Values[tag_number] = "Opera";
                                break;
                    case 104:   Values[tag_number] = "Chamber Music";
                                break;
                    case 105:   Values[tag_number] = "Sonata";
                                break;
                    case 106:   Values[tag_number] = "Symphony";
                                break;
                    case 107:   Values[tag_number] = "Booty Bass";
                                break;
                    case 108:   Values[tag_number] = "Primus";
                                break;
                    case 109:   Values[tag_number] = "Porn Groove";
                                break;
                    case 110:   Values[tag_number] = "Satire";
                                break;
                    case 111:   Values[tag_number] = "Slow Jam";
                                break;
                    case 112:   Values[tag_number] = "Club";
                                break;
                    case 113:   Values[tag_number] = "Tango";
                                break;
                    case 114:   Values[tag_number] = "Samba";
                                break;
                    case 115:   Values[tag_number] = "Folklore";
                                break;
                    case 116:   Values[tag_number] = "Ballad";
                                break;
                    case 117:   Values[tag_number] = "Power Ballad";
                                break;
                    case 118:   Values[tag_number] = "Rhythmic Soul";
                                break;
                    case 119:   Values[tag_number] = "Freestyle";
                                break;
                    case 120:   Values[tag_number] = "Duet";
                                break;
                    case 121:   Values[tag_number] = "Punk Rock";
                                break;
                    case 122:   Values[tag_number] = "Drum Solo";
                                break;
                    case 123:   Values[tag_number] = "Acapella";
                                break;
                    case 124:   Values[tag_number] = "Euro-House";
                                break;
                    case 125:   Values[tag_number] = "Dance Hall";
                                break;
                    case 255:   Values[tag_number] = "None";
                                break;
                    default:    sprintf(temp, "%i", temp[0]);
                                Values[tag_number]=temp;
                                break;
                }
            }
        }

        //Debug
        if (verbosity >= 4)
            printf("Values[%i] = %s\n", tag_number, Values[tag_number].c_str());

        tag_number++;
    }
}

//Tags are returned in Values array in this order:
//Title
//Author
//Album
//Year
//Comment
//Track Number (If ID3v1.1)
//Genre
//
//input
//=======================
//fstream& audiofile
//      previously opened mp3 file
//-----------------------
//output
//=======================
//string *Values
//      storage for ID3 tag values
void extract_ID3v1_tags(fstream& mp3file, string *Values)
{
    char temp[31];
    int i=124, j=0;

    //Parse file info
    //Get Title, Author, and Album
    for (; j<3; j++)
    {
        mp3file.seekg(i, ios_base::end);
        i -= 30;
        mp3file.readsome(temp, 30);
        temp[30]=0;
        Values[j] = temp;
    }

    //Get Year
    mp3file.seekg(i, ios_base::end);
    i -= 4;
    mp3file.readsome(temp, 4);
    temp[4]=0;
    Values[j++] = temp;

    //Get Comment and Track Number
    mp3file.seekg(i, ios_base::end);
    i -= 30;
    mp3file.readsome(temp, 30);
    temp[30]=0;
    Values[j++] = temp;
    if (temp[28] == 0)
    {
        sprintf(temp, "%i", temp[29]);
        Values[j++] = temp;
    }
    else
        j++;

    //Get Genre
    mp3file.seekg(i, ios_base::end);
    mp3file.readsome(temp, 1);
    switch (temp[0])
    {
        //Genre table
        case 0:     Values[j] = "Blues";
                    break;
        case 1:     Values[j] = "Classic Rock";
                    break;
        case 2:     Values[j] = "Country";
                    break;
        case 3:     Values[j] = "Dance";
                    break;
        case 4:     Values[j] = "Disco";
                    break;
        case 5:     Values[j] = "Funk";
                    break;
        case 6:     Values[j] = "Grunge";
                    break;
        case 7:     Values[j] = "Hip-Hop";
                    break;
        case 8:     Values[j] = "Jazz";
                    break;
        case 9:     Values[j] = "Metal";
                    break;
        case 10:    Values[j] = "New Age";
                    break;
        case 11:    Values[j] = "Oldies";
                    break;
        case 12:    Values[j] = "Other";
                    break;
        case 13:    Values[j] = "Pop";
                    break;
        case 14:    Values[j] = "R&B";
                    break;
        case 15:    Values[j] = "Rap";
                    break;
        case 16:    Values[j] = "Reggae";
                    break;
        case 17:    Values[j] = "Rock";
                    break;
        case 18:    Values[j] = "Techno";
                    break;
        case 19:    Values[j] = "Industrial";
                    break;
        case 20:    Values[j] = "Alternative";
                    break;
        case 21:    Values[j] = "Ska";
                    break;
        case 22:    Values[j] = "Death Metal";
                    break;
        case 23:    Values[j] = "Pranks";
                    break;
        case 24:    Values[j] = "Soundtrack";
                    break;
        case 25:    Values[j] = "Euro-Techno";
                    break;
        case 26:    Values[j] = "Ambient";
                    break;
        case 27:    Values[j] = "Trip-Hop";
                    break;
        case 28:    Values[j] = "Vocal";
                    break;
        case 29:    Values[j] = "Jazz+Funk";
                    break;
        case 30:    Values[j] = "Fusion";
                    break;
        case 31:    Values[j] = "Trance";
                    break;
        case 32:    Values[j] = "Classical";
                    break;
        case 33:    Values[j] = "Instrumental";
                    break;
        case 34:    Values[j] = "Acid";
                    break;
        case 35:    Values[j] = "House";
                    break;
        case 36:    Values[j] = "Game";
                    break;
        case 37:    Values[j] = "Sound Clip";
                    break;
        case 38:    Values[j] = "Gospel";
                    break;
        case 39:    Values[j] = "Noise";
                    break;
        case 40:    Values[j] = "AlternRock";
                    break;
        case 41:    Values[j] = "Bass";
                    break;
        case 42:    Values[j] = "Soul";
                    break;
        case 43:    Values[j] = "Punk";
                    break;
        case 44:    Values[j] = "Space";
                    break;
        case 45:    Values[j] = "Meditative";
                    break;
        case 46:    Values[j] = "Instrumental Pop";
                    break;
        case 47:    Values[j] = "Instrumental Rock";
                    break;
        case 48:    Values[j] = "Ethnic";
                    break;
        case 49:    Values[j] = "Gothic";
                    break;
        case 50:    Values[j] = "Darkwave";
                    break;
        case 51:    Values[j] = "Techno-Industrial";
                    break;
        case 52:    Values[j] = "Electronic";
                    break;
        case 53:    Values[j] = "Pop-Folk";
                    break;
        case 54:    Values[j] = "Eurodance";
                    break;
        case 55:    Values[j] = "Dream";
                    break;
        case 56:    Values[j] = "Southern Rock";
                    break;
        case 57:    Values[j] = "Comedy";
                    break;
        case 58:    Values[j] = "Cult";
                    break;
        case 59:    Values[j] = "Gangsta";
                    break;
        case 60:    Values[j] = "Top 40";
                    break;
        case 61:    Values[j] = "Christian Rap";
                    break;
        case 62:    Values[j] = "Pop/Funk";
                    break;
        case 63:    Values[j] = "Jungle";
                    break;
        case 64:    Values[j] = "Native American";
                    break;
        case 65:    Values[j] = "Cabaret";
                    break;
        case 66:    Values[j] = "New Wave";
                    break;
        case 67:    Values[j] = "Psychadelic";
                    break;
        case 68:    Values[j] = "Rave";
                    break;
        case 69:    Values[j] = "Showtunes";
                    break;
        case 70:    Values[j] = "Trailer";
                    break;
        case 71:    Values[j] = "Lo-Fi";
                    break;
        case 72:    Values[j] = "Tribal";
                    break;
        case 73:    Values[j] = "Acid Punk";
                    break;
        case 74:    Values[j] = "Acid Jazz";
                    break;
        case 75:    Values[j] = "Polka";
                    break;
        case 76:    Values[j] = "Retro";
                    break;
        case 77:    Values[j] = "Musical";
                    break;
        case 78:    Values[j] = "Rock & Roll";
                    break;
        case 79:    Values[j] = "Hard Rock";
                    break;
        case 80:    Values[j] = "Folk";
                    break;
        case 81:    Values[j] = "Folk-Rock";
                    break;
        case 82:    Values[j] = "National Folk";
                    break;
        case 83:    Values[j] = "Swing";
                    break;
        case 84:    Values[j] = "Fast Fusion";
                    break;
        case 85:    Values[j] = "Bebob";
                    break;
        case 86:    Values[j] = "Latin";
                    break;
        case 87:    Values[j] = "Revival";
                    break;
        case 88:    Values[j] = "Celtic";
                    break;
        case 89:    Values[j] = "Bluegrass";
                    break;
        case 90:    Values[j] = "Avantgarde";
                    break;
        case 91:    Values[j] = "Gothic Rock";
                    break;
        case 92:    Values[j] = "Progressive Rock";
                    break;
        case 93:    Values[j] = "Psychedelic Rock";
                    break;
        case 94:    Values[j] = "Symphonic Rock";
                    break;
        case 95:    Values[j] = "Slow Rock";
                    break;
        case 96:    Values[j] = "Big Band";
                    break;
        case 97:    Values[j] = "Chorus";
                    break;
        case 98:    Values[j] = "Easy Listening";
                    break;
        case 99:    Values[j] = "Acoustic";
                    break;
        case 100:   Values[j] = "Humour";
                    break;
        case 101:   Values[j] = "Speech";
                    break;
        case 102:   Values[j] = "Chanson";
                    break;
        case 103:   Values[j] = "Opera";
                    break;
        case 104:   Values[j] = "Chamber Music";
                    break;
        case 105:   Values[j] = "Sonata";
                    break;
        case 106:   Values[j] = "Symphony";
                    break;
        case 107:   Values[j] = "Booty Bass";
                    break;
        case 108:   Values[j] = "Primus";
                    break;
        case 109:   Values[j] = "Porn Groove";
                    break;
        case 110:   Values[j] = "Satire";
                    break;
        case 111:   Values[j] = "Slow Jam";
                    break;
        case 112:   Values[j] = "Club";
                    break;
        case 113:   Values[j] = "Tango";
                    break;
        case 114:   Values[j] = "Samba";
                    break;
        case 115:   Values[j] = "Folklore";
                    break;
        case 116:   Values[j] = "Ballad";
                    break;
        case 117:   Values[j] = "Power Ballad";
                    break;
        case 118:   Values[j] = "Rhythmic Soul";
                    break;
        case 119:   Values[j] = "Freestyle";
                    break;
        case 120:   Values[j] = "Duet";
                    break;
        case 121:   Values[j] = "Punk Rock";
                    break;
        case 122:   Values[j] = "Drum Solo";
                    break;
        case 123:   Values[j] = "Acapella";
                    break;
        case 124:   Values[j] = "Euro-House";
                    break;
        case 125:   Values[j] = "Dance Hall";
                    break;
        case 255:   Values[j] = "None";
                    break;
        //These only apply to ID3v2
        //case "(CR)":    Values[j] = Cover;
        //                break;
        //case "(RX)":    Values[j] = Remix;
        //                break;
        default:    sprintf(temp, "%i", temp[0]);
                    Values[j]=temp;
                    break;
    }
}

void order_audiolist(   string& audiolist,
                        string local_directory,
                        string ordering)
{
    bool default_track=true, default_album=true;
    string list, audio, audio_order="", temp_order, order="", temp_audio, extension, album, track;
    fstream audiofile;
    int length=0, ID3_version=0, outer_sort_loop=0;

    //Don't need ordering
    if (ordering == "none")
        return;

    //Copy and parse the audiolist
    list = audiolist;
    while (list.length() > 6)
    {
        //Get audio file name
        length = list.find('/');
        audio = list.substr(0, length);

        //Remove audio file from list
        list.replace(0, length+1, "");

        //Check for compatibility
        extension = audio.substr(audio.rfind('.'), length - audio.rfind('.'));

        //Debug
        if (verbosity >= 2)
            printf("\nCurrent file ==>%s\nextension ==>%s\n", audio.c_str(), extension.c_str());

        //Debug
        if (verbosity >= 4)
            printf("audiolist ==>%s\n", audiolist.c_str());

        //Debug
        if (verbosity == 1)
            printf("\n");
        if (verbosity >= 1)
            printf("File = %s\n", audio.c_str());

        //Open audio file to parse info
        if (local_directory[0] == '/')
            audiofile.open((local_directory + "/" + audio).c_str(), fstream::in | ios_base::binary);
        else
            audiofile.open((local_directory + "\\" + audio).c_str(), fstream::in | ios_base::binary);
        if (!audiofile.is_open())
        {
            cout << "Error opening" << audio << ".\n";
            cout << "Skipping ordering operations.\n";
        }
        else if (extension == ".mp3")
        {
            //Check ID3 tag version
            check_ID3_version(audiofile, ID3_version);

            if ((ID3_version & 2) == 2)
            {
                string Tags[30], Values[30];

                //extract ID3v2 tags
                extract_ID3v2_tags(audiofile, Tags, Values, 30);

                for (int i = 0; i < 30 && Tags[i][0] != 0; i++)
                {
                    if (Tags[i] == "TALB" && default_album == true)
                    {
                        album = Values[i];
                        default_album = false;
                    }
                    else if (Tags[i] == "TRCK" && default_track == true)
                    {
                        track = Values[i];
                        default_track = false;
                        //Debug
                        if (verbosity >= 4)
                        {
                            printf("Values[%i] = %s\n", i, Values[i].c_str());
                            printf("track = %s\n", track.c_str());
                        }
                    }
                }
                //Check for blank values
                if (default_album == false && album == "")
                    default_album == true;
                if (default_track == false && track == "")
                    default_track == true;

                //Debug
                if (verbosity >= 3)
                {
                    printf("album = %s\n", album.c_str());
                    printf("track = %s\n", track.c_str());
                }
            }
            else if ((ID3_version & 1) == 1)
            {
                string Values[7];

                //Parse file info
                extract_ID3v1_tags(audiofile, Values);
                album = Values[2];
                track = Values[5];

                //Check for empty strings
                if (track != "" && track != "0")
                    default_track = false;
                if (album != "")
                    default_album = false;
            }
        }
        //Close audiofile
        audiofile.close();

        if (default_track)
            track = "";
        if (default_album)
            album = "";

        //add values to order information list
        order += (track + "-" + album + 'ÿ');

        //Reset variables for next loop
        default_track = true;
        default_album = true;
    }

//Sorting algorithm is completely un-optimized (currently n^3 + n^2)
//It needs a total rewrite
//Also, it needs code additions to include sorting using both album and track data

    //Recopy the audiolist and erase it
    list = audiolist;
    audiolist = "";

    while (list.length() > 6)
    {
        string list2="", order2="", temp2_audio, temp2_order, album2, track2;
        int inner_sort_loop=0, dash;

        //Get audio file name
        length = list.find('/');
        temp_audio = list.substr(0, length);

        //Remove audio file from list
        list.replace(0, length+1, "");

        //Get audio file name
        length = order.find('ÿ');
        temp_order = order.substr(0, length);

        //Remove audio file from list
        order.replace(0, length+1, "");

        //if audiolist is blank, paste and move on
        if (audiolist == "")
        {
            audiolist = temp_audio + "/";
            audio_order = temp_order + 'ÿ';
            continue;
        }

        //if current audio file has no album and no track info, paste and move on
        if (temp_order == "-")
        {
            audiolist += (temp_audio + "/");
            audio_order += (temp_order + 'ÿ');
            continue;
        }

        //Check album and track values
        dash = temp_order.find('-');
        track = temp_order.substr(0, dash);
        album = temp_order.substr(dash + 1, temp_order.length() - (dash + 1));

        //if current audio file does not have the required sorting info, paste and move on
        if (track == "" && ordering == "track")
        {
            audiolist += (temp_audio + "/");
            audio_order += (temp_order + 'ÿ');
            continue;
        }
        if (album == "" && ordering == "album")
        {
            audiolist += (temp_audio + "/");
            audio_order += (temp_order + 'ÿ');
            continue;
        }

        //check the current order with the next and continue until a place is found or the end of the string is reached
        while (audiolist.length() > 6)
        {
            //Get audio file name
            length = audiolist.find('/');
            temp2_audio = audiolist.substr(0, length);

            //Remove audio file from list
            audiolist.replace(0, length+1, "");

            //Get audio file name
            length = audio_order.find('ÿ');
            temp2_order = audio_order.substr(0, length);

            //Remove audio file from list
            audio_order.replace(0, length+1, "");

            //if current audio file has no album and no track info, paste and move on
            if (temp2_order == "-")
            {
                audiolist = (temp2_audio + "/") + audiolist;
                audio_order = (temp2_order + 'ÿ') + audio_order;
                break;
            }

            //Check album and track values
            dash = temp2_order.find('-');
            track2 = temp2_order.substr(0, dash);
            album2 = temp2_order.substr(dash + 1, temp2_order.length() - (dash + 1));

            //Debug
            if (verbosity >= 5)
                printf ("track=%s\nalbum=%s\ntrack2=%s\nalbum2=%s\n", track.c_str(), album.c_str(), track2.c_str(), album2.c_str());

            if (!track2.length())
            {
                //album data
                if (ordering == "album")
                {
                    //Compare and break or move on
                    if (album < album2)
                    {
                        audiolist = (temp2_audio + "/") + audiolist;
                        audio_order = (temp2_order + 'ÿ') + audio_order;
                        break;
                    }
                }
                else if (ordering == "both")
                {
                    if (album < album2)
                    {
                        audiolist = (temp2_audio + "/") + audiolist;
                        audio_order = (temp2_order + 'ÿ') + audio_order;
                        break;
                    }
                    else if (album == album2)
                    {
                        if (track.length() > 0)
                        {
                            audiolist = (temp2_audio + "/") + audiolist;
                            audio_order = (temp2_order + 'ÿ') + audio_order;
                            break;
                        }
                    }
                }
                else
                {
                    //Otherwise paste and move on
                    audiolist = (temp2_audio + "/") + audiolist;
                    audio_order = (temp2_order + 'ÿ') + audio_order;
                    break;
                }
            }
            else if (!album2.length())
            {
                //track data
                if (ordering == "track")
                {
                    //Compare and break or move on
                    if (atoi(track.c_str()) < atoi(track2.c_str()))
                    {
                        audiolist = (temp2_audio + "/") + audiolist;
                        audio_order = (temp2_order + 'ÿ') + audio_order;
                        break;
                    }
                }
                else if (ordering == "both")
                {
                    if (!album.length())
                    {
                        //Compare and break or move on
                        if (atoi(track.c_str()) < atoi(track2.c_str()))
                        {
                            audiolist = (temp2_audio + "/") + audiolist;
                            audio_order = (temp2_order + 'ÿ') + audio_order;
                            break;
                        }
                    }
                }
                else
                {
                    //Otherwise paste and move on
                    audiolist = (temp2_audio + "/") + audiolist;
                    audio_order = (temp2_order + 'ÿ') + audio_order;
                    break;
                }
            }
            else
            {
                //Both track and album data
                if (ordering == "track")
                {
                    //Compare and break or move on
                    if (atoi(track.c_str()) < atoi(track2.c_str()))
                    {
                        audiolist = (temp2_audio + "/") + audiolist;
                        audio_order = (temp2_order + 'ÿ') + audio_order;
                        break;
                    }
                }
                else if (ordering == "album")
                {
                    //Compare and break or move on
                    if (album < album2)
                    {
                        audiolist = (temp2_audio + "/") + audiolist;
                        audio_order = (temp2_order + 'ÿ') + audio_order;
                        break;
                    }
                }
                else if (ordering == "both")
                {
                    //Compare and break or move on
                    if (album < album2)
                    {
                        audiolist = (temp2_audio + "/") + audiolist;
                        audio_order = (temp2_order + 'ÿ') + audio_order;
                        break;
                    }
                    else if (album == album2)
                    {
                        //Compare and break or move on
                        if (atoi(track.c_str()) < atoi(track2.c_str()))
                        {
                            audiolist = (temp2_audio + "/") + audiolist;
                            audio_order = (temp2_order + 'ÿ') + audio_order;
                            break;
                        }
                    }
                }
            }

            //not yet time to paste, return to list
            list2 += (temp2_audio + "/");
            order2 += (temp2_order + 'ÿ');

            //Debug
            if (verbosity >= 4)
            {
                inner_sort_loop++;
                printf ("Inner sort loop #%i\n", inner_sort_loop);
            }
        }

        //Insert the current file into the list
        audiolist = list2 + (temp_audio + "/") + audiolist;
        audio_order = order2 + (temp_order + 'ÿ') + audio_order;

        //Debug
        if (verbosity >= 4)
        {
            outer_sort_loop++;
            printf ("Outer sort loop #%i\n", outer_sort_loop);
        }
    }
}

void add_audiofile( fstream& xmlfile,
                    string& audiolist,
                    string local_directory,
                    string web_directory,
                    string date,
                    bool mp4_is_video)
{
    bool default_title = true, default_author = true, default_description = true;
    string audio, extension, link, linktemp, title, author, description;
    char temp[1024];
    fstream audiofile;
    int length=0, ID3_version=0;

    //Get audio file name
    length = audiolist.find('/');
    audio = audiolist.substr(0, length);

    //Remove audio file from list
    audiolist.replace(0, length+1, "");

    //Check for compatibility
    extension = audio.substr(audio.rfind('.'), length - audio.rfind('.'));

    //Debug
    if (verbosity >= 2)
        printf("\nCurrent file ==>%s\nextension ==>%s\n", audio.c_str(), extension.c_str());

    //Debug
    if (verbosity >= 4)
        printf("audiolist ==>%s\n", audiolist.c_str());

    //Debug
    if (verbosity == 1)
        printf("\n");
    if (verbosity >= 1)
        printf("File = %s\n", audio.c_str());

    //Open audio file to parse info
    if (local_directory[0] == '/')
        audiofile.open((local_directory + "/" + audio).c_str(), fstream::in | ios_base::binary);
    else
        audiofile.open((local_directory + "\\" + audio).c_str(), fstream::in | ios_base::binary);
    if (!audiofile.is_open())
    {
        cout << "Error opening" << audio << ".\n";
        cout << "Using defaults.\n";
    }
    else if (extension == ".mp3")
    {
        //Check ID3 tag version
        check_ID3_version(audiofile, ID3_version);

        if ((ID3_version & 2) == 2)
        {
            string Tags[30], Values[30];

            author = "";
            title = "";
            description = "";

            //extract ID3v2 tags
            extract_ID3v2_tags(audiofile, Tags, Values, 30);

            for (int i = 0; i < 30 && Tags[i][0] != 0; i++)
            {
                if (Tags[i] == "TIT2" && default_title == true)
                {
                    title = Values[i];
                    default_title = false;
                }
                else if (Tags[i] == "TPE1" && default_author == true)
                {
                    author = Values[i];
                    default_author = false;
                }
                else if (Tags[i] == "TCON" && default_description == true)
                {
                    description = Values[i];
                    default_description = false;
                }
            }
            //Check for blank values
            if (default_author == false && author == "")
                default_author == true;
            if (default_title == false && title == "")
                default_title == true;
            if (default_description == false && description == "")
                default_description == true;

            //Debug
            if (verbosity >= 1)
            {
                printf("author = %s\n", author.c_str());
                printf("title = %s\n", title.c_str());
                printf("description = %s\n", description.c_str());
            }
        }
        else if ((ID3_version & 1) == 1)
        {
            string Values[7];

            author = "";
            title = "";
            description = "";

            //Parse file info
            extract_ID3v1_tags(audiofile, Values);
            title = Values[0];
            author = Values[1];
            description = Values[6];

            //Check for empty strings
            if (title != "")
                default_title = false;
            if (author != "")
                default_author = false;
            if (description != "")
                default_description = false;
        }

        //Reset shared variables
        temp[0] = 0;
    }
    //Close audiofile
    audiofile.close();

    //Add entry to channel
    xmlfile << "  <item>\n";

    //Debug
    if (verbosity >= 3)
        cout << "Output for Current File:\n  <item>\n";

    //Check title strings for special characters
    if (default_title)
        replace_special(linktemp, audio.substr(0, length-4));
    else
        replace_special(linktemp, title);

    //Add title
    xmlfile << "   <title>" << linktemp << "</title>\n";

    //Debug
    if (verbosity >= 3)
        cout << "   <title>" << linktemp << "</title>\n";

    //Check file strings for special characters
    replace_special(linktemp, web_directory);
    link = linktemp;
    link += '/';
    replace_special(linktemp, audio);
    link += linktemp;

    //Add link
    xmlfile << "   <link>" << link << "</link>\n";

    //Debug
    if (verbosity >= 3)
        cout << "   <link>" << link << "</link>\n";

    //Add description if ID3 tag exists
    if (!default_description)
    {
        xmlfile << "   <description>" << description << "</description>\n";

        //Debug
        if (verbosity >= 3)
            cout << "   <description>" << description << "</description>\n";
    }

    //Add author if ID3 tag exists
    if (!default_author)
    {
        xmlfile << "   <author>" << author << "</author>\n";

        //Debug
        if (verbosity >= 3)
            cout << "   <author>" << author << "</author>\n";
    }

    //Add Date
    xmlfile << "   <pubDate>" << date << "</pubDate>\n";

    //Debug
    if (verbosity >= 3)
        cout << "   <pubDate>" << date << "</pubDate>\n";

    //Check file strings for spaces
    encode_url(linktemp, web_directory);
    link = linktemp;
    link += '/';
    encode_url(linktemp, audio);
    link += linktemp;

    //Add file
    xmlfile << "   <enclosure url=\"" << link << "\" type=\"";

    //Debug
    if (verbosity >= 3)
        cout << "   <enclosure url=\"" << link << "\" type=\"";

    //Add type
    if (extension == ".mp3" ||
        extension == ".m4a")
    {
        xmlfile << "audio/" << extension.substr(1, extension.length() - 1);

        //Debug
        if (verbosity >= 3)
            cout << "audio/" << extension.substr(1, extension.length() - 1);
    }
    else if (extension == ".mp4" && !mp4_is_video)
    {
        xmlfile << "audio/mp4";

        //Debug
        if (verbosity >= 3)
            cout << "audio/mp4";
    }
    else if (extension == ".mp4" && mp4_is_video)
    {
        xmlfile << "video/mp4";

        //Debug
        if (verbosity >= 3)
            cout << "video/mp4";
    }
    else if (extension == ".m4v")
    {
        xmlfile << "video/" << extension.substr(1, extension.length() - 1);

        //Debug
        if (verbosity >= 3)
            cout << "video/" << extension.substr(1, extension.length() - 1);
    }
    else if (extension == ".jpeg" ||
             extension == ".jpg" ||
             extension == ".gif" ||
             extension == ".png" ||
             extension == ".tiff" ||
             extension == ".tif" ||
             extension == ".bmp")
    {
        xmlfile << "image/" << extension.substr(1, extension.length() - 1);

        //Debug
        if (verbosity >= 3)
            cout << "image/" << extension.substr(1, extension.length() - 1);
    }

    //Finish item
    xmlfile << "\"/>\n";

    //Debug
    if (verbosity >= 3)
        cout << "\"/>\n";

    //Close entry
    xmlfile << "  </item>\n";

    //Debug
    if (verbosity >= 3)
        cout << "  </item>\n";
}

void close_xml_file(fstream& xmlfile)
{
    //Setup XML footer
    xmlfile << " </channel>\n";

    //Add rss close tag if none exists
    xmlfile.peek();
    if (xmlfile.eof())
    {
        //Clear eof bit
        xmlfile.clear();

        xmlfile << "</rss>";
    }

    //close xml file
    xmlfile.close();
}

//Set Date so it will remain constant throughout process
void set_date(  string UTC_offset,
                string& date)
{
    char temp[1024];
    char * weekday[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    char * month[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    tm *t_date;
    time_t rawtime;

    //Format Date
    //Get Timezone offset
    time(&rawtime);
    t_date = localtime(&rawtime);

    //Day of week
    date = weekday[t_date->tm_wday];
    date += ", ";

    //Day of Month
    sprintf(temp, "%d", t_date->tm_mday);
    date += temp;
    date += " ";

    //Month
    date += month[t_date->tm_mon];
    date += " ";

    //Year
    sprintf(temp, "%04d", t_date->tm_year + 1900);
    date += temp;
    date += " ";

    //Hour
    sprintf(temp, "%02d", t_date->tm_hour);
    date += temp;
    date += ":";

    //Minute
    sprintf(temp, "%02d", t_date->tm_min);
    date += temp;
    date += ":";

    //Second
    sprintf(temp, "%02d", t_date->tm_sec);
    date += temp;
    date += " ";

    //Timezone offset
    date += UTC_offset;
}

void insert_CHAN_tag(fstream& PSPfile, string tag, string value)
{
    char temp2[1024];
    int length=value.length(), i=0;

    //insert tag and entry size
    strcpy(temp2, tag.c_str());
    PSPfile.write(temp2, 4);
    PSPfile.put(length & 0xFF);
    PSPfile.put((length & 0xFF00)>>8);
    PSPfile.put((length & 0xFF0000)>>16);
    PSPfile.put((length & 0xFF000000)>>24);

    //insert entry
    while (length >= 1023)
    {
        strcpy(temp2, value.substr(i, 1023).c_str());
        length -= 1023;
        i += 1023;
        PSPfile.write(temp2, 1023);
    }
    if (length > 0)
    {
        strcpy(temp2, value.substr(i, length).c_str());
        PSPfile.write(temp2, length);
    }
}

int add_rss(    string PSP_path, 
                string xmlfilename, 
                string web_directory,
                string directory,
                string image, 
                string title, 
                string description, 
                string copyright)
{
    fstream PSPfile, imagefile, CHANimagefile;
    FILE *file;
    string temp, PSPpath, CHLIST="", image_name="", CHAN_image_name="", CHAN_image_path="", URL="", url, CHAN_URL="", chan_url;
    char temp2[1024];
    int length=0, random=0, last_op=1024, channel_length=0;

    //Prep URL entry
    encode_url(temp, web_directory);
    URL = temp;
    encode_url(temp, xmlfilename);
    URL += "/" + temp;

    //Check system type and add path
    if (PSP_path[0] == '/')
    {
        PSPpath = PSP_path + "/PSP/SYSTEM/RSSCH/CHANNELS";
        CHLIST = PSPpath + "/CHLIST";
    }
    else
    {
        PSPpath = PSP_path + ":\\PSP\\SYSTEM\\RSSCH\\CHANNELS";
        CHLIST = PSPpath + "\\CHLIST";
    }

    //Check for existence of file.
    file = fopen(CHLIST.c_str(), "a");
    fclose(file);
    PSPfile.open(CHLIST.c_str(), fstream::in|fstream::out|ios_base::binary);
    if (!PSPfile.is_open())
    {
        cout << "Error opening PSP RSS file: " << CHLIST << ".\n";
        return 1;
    }

    //If file already exists, check for URL and seek to the end.
    PSPfile.peek();
    if (PSPfile.eof())
    {
        //Clear eof bit
        PSPfile.clear();

        //Setup CHLIST header
        PSPfile.write("CSFF", 4);
        PSPfile.put(0);
        PSPfile.put(1);
        PSPfile.put(0);
        PSPfile.put(0);
    }
    else
    {
        //Reset pointer
        PSPfile.seekg(0, ios_base::beg);

        //Skip header tag "CSFF"
        PSPfile.readsome(temp2, 8);

        //Debug
        if (verbosity >= 4)
            printf("CSFF tag\n========\ntemp2=0x%02X%02X%02X%02X%02X%02X%02X%02X\n", temp2[0] & 0xFF, temp2[1] & 0xFF, temp2[2] & 0xFF, temp2[3] & 0xFF, temp2[4] & 0xFF, temp2[5] & 0xFF, temp2[6] & 0xFF, temp2[7] & 0xFF);

        //Scan for URL match
        while (1)
        {
            //check if last run hit the end
            PSPfile.peek();
            if (PSPfile.eof())
            {
                PSPfile.clear();
                break;
            }

            //Check for CHAN header
            PSPfile.readsome(temp2, 4);

            //Debug
            if (verbosity >= 4)
                printf("CHAN tag\n========\ntemp2=0x%02X%02X%02X%02X\n", temp2[0] & 0xFF, temp2[1] & 0xFF, temp2[2] & 0xFF, temp2[3] & 0xFF);

            temp2[4] = 0;

            //Debug
            if (verbosity >= 4)
                printf("temp2=%s\n", temp2);

            temp = temp2;
            if (temp != "CHAN")
            {
                cout << "Format confusion in PSP RSS file: " << CHLIST << ".\nCould not add channel. You need to do so manually.\n";
                PSPfile.close();

                //Debug
                if (verbosity >= 4)
                    printf("CHAN Header\n===========\ntemp=%s\ntemp2=%s\n", temp.c_str(), temp2);

                return 1;
            }

            //Get total length of channel entry
            PSPfile.readsome(temp2, 4);
            channel_length = temp2[0] & 0xFF;
            channel_length += (temp2[1] & 0xFF)<<8;
            channel_length += (temp2[2] & 0xFF)<<16;
            channel_length += (temp2[3] & 0xFF)<<24;

            //Debug
            if (verbosity >= 4)
                printf("channel_length=0x%X\n", channel_length);

            //Check for URL header
            PSPfile.readsome(temp2, 4);
            channel_length -= 4;
            temp2[4] = 0;
            temp = temp2;
            if (temp != "URL ")
            {
                cout << "Format confusion in PSP RSS file: " << CHLIST << ".\nCould not add channel. You need to do so manually.\n";
                PSPfile.close();

                //Debug
                if (verbosity >= 4)
                    printf("URL Header\n==========\ntemp=%s\ntemp2=%s\n", temp.c_str(), temp2);

                return 1;
            }

            //Get length of URL
            PSPfile.readsome(temp2, 4);
            channel_length -= 4;
            length = temp2[0] & 0xFF;
            length += (temp2[1] & 0xFF)<<8;
            length += (temp2[2] & 0xFF)<<16;
            length += (temp2[3] & 0xFF)<<24;
   
            //Debug
            if (verbosity >= 4)
                printf("length=0x%X\n", length);

            //Get URL
            CHAN_URL="";
            while (length >= 1023)
            {
                //Get part and add to whole
                PSPfile.readsome(temp2, 1023);
                channel_length -= 1023;
                length -= 1023;
                temp2[1023]=0;
                CHAN_URL += temp2;
            }
            if (length > 0)
            {
                //Get part and add to whole
                PSPfile.readsome(temp2, length);
                channel_length -= length;
                temp2[length]=0;
                length = 0;
                CHAN_URL += temp2;
            }

            //Debug
            if (verbosity >= 4)
                printf("URL=%s\nCHAN_URL%s\n", URL.c_str(), CHAN_URL.c_str());

            //Make lowercase copies of URLs
            url = URL;
            chan_url = CHAN_URL;
            string_tolower(url);
            string_tolower(chan_url);

            //Compare URLs
            if (url == chan_url)
            {
                cout << "Channel is already present in PSP RSS file. No need to add.\n";
                PSPfile.close();
                return 0;
            }
        
            //Go to next CHAN header
            PSPfile.seekg(channel_length, ios_base::cur);
        }

    //Since the URL was not found, seek the put pointer to the end of the file
    PSPfile.seekp(0, ios_base::end);
    }

    //Format CHAN entry
    if (image.find('/') == string::npos && image.find('\\') == string::npos)
    {
        //Check for existence of image file
        if (directory[0] == '/')
            imagefile.open((directory + "/" + image).c_str(), fstream::in|ios_base::binary);
        else
            imagefile.open((directory + "\\" + image).c_str(), fstream::in|ios_base::binary);
        if (!imagefile.is_open())
        {
            cout << "Error opening image file:" << image << ". Cannot add to PSP.\n It will automatically download next time you load this channel.\n";
        }
        else
        {
            //Try random filenames until one works
            while (1)
            {
                //generate random (40 characters + extension) image name
                CHAN_image_name = "";
                for (int i=0; i<10; i++)
                {
                    random = rand()%0x10000;
                    sprintf(temp2, "%04x", random & 0xFFFF);
                    CHAN_image_name += temp2;
                }

                //Add extension
                CHAN_image_name += image.substr(image.rfind('.'), image.length() - image.rfind('.'));

                //Check system type and add path
                if (PSP_path[0] == '/')
                    CHAN_image_path = PSPpath + '/' + CHAN_image_name;
                else
                    CHAN_image_path = PSPpath + '\\' + CHAN_image_name;

                //Make sure file does not exist
                file = fopen(CHAN_image_path.c_str(), "a");
                fclose(file);
                CHANimagefile.open(CHAN_image_path.c_str(), fstream::in|fstream::out|ios_base::binary);
                if (!CHANimagefile.is_open())
                {
                    cout << "Error opening image file on PSP. Cannot add to PSP.\n It will automatically download next time you load this channel.\n";
                    CHAN_image_name = "";
                    break;
                }

                //If file already exists, try again.
                CHANimagefile.peek();
                if (CHANimagefile.eof())
                {
                    //Clear eof bit
                    CHANimagefile.clear();

                    //Copy image file
                    CHANimagefile << imagefile.rdbuf();

                    //Copied image file so close both and break out of while loop
                    imagefile.close();
                    CHANimagefile.close();
                    break;
                }
                else
                    CHANimagefile.close();
            }
        }
    }

    //Add lengths of entries
    channel_length =    URL.length() + 
                        title.length() + 
                        description.length() + 
                        copyright.length() + 
                        CHAN_image_name.length() +
                        40;     //5 4-byte size entries + 5 4-byte tags

    //Insert into file CHAN entry into file
    strcpy(temp2, "CHAN");
    PSPfile.write(temp2, 4);
    PSPfile.put(channel_length & 0xFF);
    PSPfile.put((channel_length & 0xFF00)>>8);
    PSPfile.put((channel_length & 0xFF0000)>>16);
    PSPfile.put((channel_length & 0xFF000000)>>24);

    //Add URL
    insert_CHAN_tag(PSPfile, "URL ", URL);

    //Add TITL
    insert_CHAN_tag(PSPfile, "TITL", title);

    //Add DESC
    insert_CHAN_tag(PSPfile, "DESC", description);

    //Add COPY
    insert_CHAN_tag(PSPfile, "COPY", copyright);

    //Add IMAG
    insert_CHAN_tag(PSPfile, "IMAG", CHAN_image_name);

    //Close file
    PSPfile.close();
}

int main(int argc, char* argv[])
{
    fstream     xmlfile, videofile;
    string      xmlfilename,
                videofilename,
                web_directory,
                local_directory,
                image="",
                audiolist,
                title,
                link="",
                description="",
                copyright="",
                UTC_offset="+0000",
                special,
                date,
                ordering="none",
                movie_order="none",
                firmware="2.80",
                audio,
                extension,
                PSP_path="";
    int         firmware_major=2,
                firmware_minor=80,
                error=0,
                i=1,
                temp=0,
                length;
    bool        first_video=true,
                image_set=false;

    //If not enough parameters, say so
    if (argc < 2)
    {
        cout << "Gimme a break here. I need input.\n";
        cout << "Some of PSP_RSS_feed assumes you are entering values correctly.\n\n";
        cout << "Unix/Linux:\n";
        cout << "Command prompt>PSP_RSS_feed [-U] -O <xml output file> -W <web file directory>\n";
        cout << "\t-L <local file directory> -T <channel title>\n";
        cout << "\t[-Z <time zone offset>] [-C <channel link>] [-D <channel description>]\n";
        cout << "\t[-R <channel copyright>] [-I <channel image filename>]\n";
        cout << "\t[-S <album-track sorting>] [-M <movie sorting>] [-P <Path to PSP>]\n";
        cout << "\t[-V <debug verbosity>]\n\n";
        cout << "DOS/Windows:\n";
        cout << "Command prompt>PSP_RSS_feed [-U] @inputfile\n";
        cout << "Where inputfile has one line containing:\n";
        cout << "\t-O <xml output file> -W <web file directory>\n";
        cout << "\t-L <local file directory> -T <channel title>\n";
        cout << "\t[-Z <time zone offset>] [-C <channel link>] [-D <channel description>]\n";
        cout << "\t[-R <channel copyright>] [-I <channel image filename>]\n";
        cout << "\t[-S <album-track sorting>] [-M <movie sorting>] [-P <PSP Drive Letter>]\n";
        cout << "\t[-V <debug verbosity>]\n\n";
        cout << "Output will set xml links to \n";
        cout << "\t\"<web file directory>/each audio file in<local file directory>\"\n\n";
        cout << "If specified, output will set image link to \n";
        cout << "\t\"<web file directory>/<channel image filename>\"\n";
        cout << "\tOtherwise the image link will be set to the first image found.\n\n";
        cout << "The time zone offset is in [+-]hhmm format (i.e., -0600)\n";
        cout << "\nAlbum-track sorting must have a value of either \"album\", \"track\",\n";
        cout << "\t\"both\", or \"none\".\n";
        cout << "Movie sorting must have a value of either \"file\", \"file_separate\",\n";
        cout << "\t\"channel\", \"channel_separate\", \"separate\", or \"none\".For \"file\",\n";
        cout << "\ta 2 will be appeneded beforethe extension.\n";
        cout << "\nFor a User Input Prompt, add -U as the only parameter.\n";
        cout << "For a version information, add --version as the only parameter.\n";
        return 1;
    }

    //Debug
    printf("Begin parsing input.\n");

    //If first argument is user interface, prompt for values
    if (strcmp(argv[i], "-U") == 0 || strcmp(argv[i], "-u") == 0)
    {
#ifdef _UNIX_
        system("clear");
#else
        system("cls");
#endif
        cout << "Output File Name (XML) ==>\n";
        getline (cin, xmlfilename);
        cout << "Web Directory (where files are stored online; no ending \"/\") ==>\n";
        getline (cin, web_directory);
        cout << "Local Directory (where files are stored locally; no ending \"/\") ==>\n";
        getline (cin, local_directory);
        cout << "Channel Title ==>\n";
        getline (cin, title);
        cout << "Time Zone Offset ([+-]hhmm format [i.e., -0600]) ==>\n";
        getline (cin, UTC_offset);
        cout << "Channel Hyper Link ==>\n";
        getline (cin, link);
        cout << "Channel Description ==>\n";
        getline (cin, description);
        cout << "Channel Copyright Information ==>\n";
        getline (cin, copyright);
        cout << "Channel Image File (displayed in channel list on PSP) ==>\n";
        getline (cin, image);
        cout << "Channel Sorting Method (\"album\", \"track\", \"both\", or \"none\") ==>\n";
        getline (cin, ordering);
        if (ordering == "")
            ordering = "none";
        if (ordering != "track" && ordering != "album" && ordering != "both" && ordering != "none")
        {
            cout << "Parameter " << ordering << " must be one of \"album\", \"track\", \"both\", or \"none\". Ignoring.\n";
            ordering = "none";
        }
        cout << "Movie Sorting Method (\"file\", \"file_separate\", \"channel\", \"channel_separate\",\n";
        cout << "\"separate\", or \"none\") ==>\n";
        getline (cin, movie_order);
        if (movie_order == "")
            movie_order = "none";
        if (movie_order != "file" && movie_order != "file_separate" && movie_order != "channel" && movie_order != "channel_separate" && movie_order != "separate" && movie_order != "none")
        {
            cout << "Parameter " << movie_order << " must be one of \"file\", \"file_separate\",\n";
            cout << "\"channel\", \"channel_separate\", \"separate\", or \"none\". Ignoring.\n";
            movie_order = "none";
        }
        cout << "PSP Firmware Version (major#.minor#) ==>\n";
        getline (cin, firmware);
        temp = firmware.find('.');
        if (firmware.find('.') == string::npos)
        {
            cout << "Parameter " << firmware << " must be the version of your PSP firmware (major#.minor#). Ignoring.\n";
            firmware = "2.80";
            temp = firmware.find('.');
        }
        firmware_major = atoi(firmware.substr(0, temp).c_str());
        firmware_minor = atoi(firmware.substr(temp + 1, firmware.length() - (temp + 1)).c_str());
        if (firmware.length() < 4)
            firmware_minor *= 10;
        if (firmware_major < 2 || (firmware_major == 2 && firmware_minor < 60))
        {
            cout << "Your PSP does not support RSS feeds. Aborting.\n";
            cout << "If you are generating an RSS feed for another PSP,\n";
            cout << "enter that PSP's firmware version or do not specify a version.\n";
            return 1;
        }
        cout << "PSP Drive Letter (DOS/Windows) ==>\nPath to PSP (Linux/Unix) ==>\n";
        getline (cin,  PSP_path);
    }
    else if (argv[i][0] != '-')
    {
        if (argc < 5)
        {
            cout << "Gimme a break here. I need input.\n\n";
            cout << "Unix/Linux:\n";
            cout << "Command prompt>PSP_RSS_feed [-U] <xml output file> <web file directory>\n";
            cout << "\t<local file directory> <channel title> <time zone offset>\n";
            cout << "\t[<channel link> [<channel description> [<channel copyright>\n";
            cout << "\t[<channel image filename>]]]]\n\n";
            cout << "DOS/Windows:\n";
            cout << "Command prompt>PSP_RSS_feed [-U] @inputfile\n";
            cout << "Where inputfile has one line containing:\n";
            cout << "\t<xml output file> <web file directory> <local file directory>\n";
            cout << "\t<channel title> <time zone offset>\n";
            cout << "\t[<channel link> [<channel description> [<channel copyright>\n";
            cout << "\t[<channel image filename>]]]]\n\n";
            cout << "Output will set xml links to \n";
            cout << "\t\"<web file directory>/each audio file in<local file directory>\"\n";
            cout << "If specified, output will set image link to \n";
            cout << "\t\"<web file directory>/<channel image filename>\"\n";
            cout << "\tOtherwise the image link will be set to the first image found.\n";
            cout << "The time zone offset is in [+-]hhmm format (i.e., -0600)\n";
            cout << "\nFor a User Input Prompt, add -U as the only parameter.\n";
            return 1;
        }
        else
        {
            //Scan command line for tags
            xmlfilename = argv[i++];
            web_directory = argv[i++];
            local_directory = argv[i++];
            title = argv[i++];
            if (argc > 5)
                UTC_offset = argv[i++];
            if (argc > 6)
                link = argv[i++];
            if (argc > 7)
                description = argv[i++];
            if (argc > 8)
                copyright = argv[i++];

            //Check for image file
            if (argc > 9)
                image = argv[i];
        }
    }
    else
    {
        while (i < argc)
        {
            if (strcmp(argv[i], "-L") == 0 || strcmp(argv[i], "-l") == 0)
            {
                i++;
                if (i == argc)
                {
                    cout << "Missing Value for " << argv[i - 1] << ".\n";
                    return 1;
                }
                local_directory = argv[i++];
            }
            else if (strcmp(argv[i], "-O") == 0 || strcmp(argv[i], "-o") == 0)
            {
                i++;
                if (i == argc)
                {
                    cout << "Missing Value for " << argv[i - 1] << ".\n";
                    return 1;
                }
                xmlfilename = argv[i++];
            }
            else if (strcmp(argv[i], "-W") == 0 || strcmp(argv[i], "-w") == 0)
            {
                i++;
                if (i == argc)
                {
                    cout << "Missing Value for " << argv[i - 1] << ".\n";
                    return 1;
                }
                web_directory = argv[i++];
            }
            else if (strcmp(argv[i], "-T") == 0 || strcmp(argv[i], "-t") == 0)
            {
                i++;
                if (i == argc)
                {
                    cout << "Missing Value for " << argv[i - 1] << ".\n";
                    return 1;
                }
                title = argv[i++];
            }
            else if (strcmp(argv[i], "-Z") == 0 || strcmp(argv[i], "-z") == 0)
            {
                i++;
                if (i == argc)
                {
                    cout << "Missing Value for " << argv[i - 1] << ".\n";
                    return 1;
                }
                UTC_offset = argv[i++];
            }
            else if (strcmp(argv[i], "-C") == 0 || strcmp(argv[i], "-c") == 0)
            {
                i++;
                if (i == argc)
                {
                    cout << "Missing Value for " << argv[i - 1] << ".\n";
                    return 1;
                }
                link = argv[i++];
            }
            else if (strcmp(argv[i], "-D") == 0 || strcmp(argv[i], "-d") == 0)
            {
                i++;
                if (i == argc)
                {
                    cout << "Missing Value for " << argv[i - 1] << ".\n";
                    return 1;
                }
                description = argv[i++];
            }
            else if (strcmp(argv[i], "-R") == 0 || strcmp(argv[i], "-r") == 0)
            {
                i++;
                if (i == argc)
                {
                    cout << "Missing Value for " << argv[i - 1] << ".\n";
                    return 1;
                }
                copyright = argv[i++];
            }
            else if (strcmp(argv[i], "-I") == 0 || strcmp(argv[i], "-i") == 0)
            {
                i++;
                if (i == argc)
                {
                    cout << "Missing Value for " << argv[i - 1] << ".\n";
                    return 1;
                }
                image = argv[i++];
            }
            else if (strcmp(argv[i], "-V") == 0 || strcmp(argv[i], "-v") == 0)
            {
                i++;
                if (i == argc)
                {
                    cout << "Missing Value for " << argv[i - 1] << ".\n";
                    return 1;
                }
                verbosity = atoi(argv[i++]);
            }
            else if (strcmp(argv[i], "-P") == 0 || strcmp(argv[i], "-p") == 0)
            {
                i++;
                if (i == argc)
                {
                    cout << "Missing Value for " << argv[i - 1] << ".\n";
                    return 1;
                }
                firmware = argv[i++];
                temp = firmware.find('.');
                if (firmware.find('.') == string::npos)
                {
                    cout << "Parameter " << argv[i-1] << " must be the version of your PSP firmware (major#.minor#). Ignoring.\n";
                    firmware = "2.80";
                    temp = firmware.find('.');
                }
                firmware_major = atoi(firmware.substr(0, temp).c_str());
                firmware_minor = atoi(firmware.substr(temp + 1, firmware.length() - (temp + 1)).c_str());
                if (firmware.length() < 4)
                    firmware_minor *= 10;
                if (firmware_major < 2 || (firmware_major == 2 && firmware_minor < 60))
                {
                    cout << "Your PSP does not support RSS feeds. Aborting.\n";
                    cout << "If you are generating an RSS feed for another PSP,\n";
                    cout << "enter that PSP's firmware version or do not specify a version.\n";
                    return 1;
                }
            }
            else if (strcmp(argv[i], "-S") == 0 || strcmp(argv[i], "-s") == 0)
            {
                i++;
                if (i == argc)
                {
                    cout << "Missing Value for " << argv[i - 1] << ".\n";
                    return 1;
                }
                ordering = argv[i++];
                if (ordering != "track" && ordering != "album" && ordering != "both" && ordering != "none")
                {
                    cout << "Parameter " << argv[i-1] << " must be one of \"album\", \"track\", \"both\", or \"none\". Ignoring.\n";
                    ordering = "none";
                }
            }
            else if (strcmp(argv[i], "-M") == 0 || strcmp(argv[i], "-m") == 0)
            {
                i++;
                if (i == argc)
                {
                    cout << "Missing Value for " << argv[i - 1] << ".\n";
                    return 1;
                }
                movie_order = argv[i++];
                if (movie_order != "file" && movie_order != "file_separate" && movie_order != "channel" && movie_order != "channel_separate" && movie_order != "separate" && movie_order != "none")
                {
                    cout << "Parameter " << argv[i-1] << " must be one of \"file\", \"file_separate\",\n";
                    cout << "\"channel\", \"channel_separate\", \"separate\", or \"none\". Ignoring.\n";
                    movie_order = "none";
                }
            }
            else if (strcmp(argv[i], "-P") == 0 || strcmp(argv[i], "-p") == 0)
            {
                i++;
                if (i == argc)
                {
                    cout << "Missing Value for " << argv[i - 1] << ".\n";
                    return 1;
                }
                PSP_path = argv[i++];
            }
            else if (strcmp(argv[i], "--version") == 0)
            {
                cout << "PSP_RSS_feed\n\nPSP RSS feed Generator 2.1\n";
                cout << "(c) 2006 Christopher James Meacham (alias Optimus[ Prime])\n";
                cout << "Program and source subject to the 2004 Fair License (rhid.com/fair)";
                return 0;
            }
            else if (   strcmp(argv[i], "-H") == 0 ||
                        strcmp(argv[i], "-h") == 0 ||
                        strcmp(argv[i], "-?") == 0 ||
                        strcmp(argv[i], "/?") == 0 ||
                        strcmp(argv[i], "--help") == 0)
            {
                cout << "Some of PSP_RSS_feed assumes you are entering values correctly.\n\n";
                cout << "Unix/Linux:\n";
                cout << "Command prompt>PSP_RSS_feed [-U] -O <xml output file> -W <web file directory>\n";
                cout << "\t-L <local file directory> -T <channel title>\n";
                cout << "\t[-Z <time zone offset>] [-C <channel link>] [-D <channel description>]\n";
                cout << "\t[-R <channel copyright>] [-I <channel image filename>]\n";
                cout << "\t[-S <album-track sorting>] [-M <movie sorting>] [-A <Path to PSP>]\n";
                cout << "\t[-V <debug verbosity>]\n\n";
                cout << "DOS/Windows:\n";
                cout << "Command prompt>PSP_RSS_feed [-U] @inputfile\n";
                cout << "Where inputfile has one line containing:\n";
                cout << "\t-O <xml output file> -W <web file directory>\n";
                cout << "\t-L <local file directory> -T <channel title>\n";
                cout << "\t[-Z <time zone offset>] [-C <channel link>] [-D <channel description>]\n";
                cout << "\t[-R <channel copyright>] [-I <channel image filename>]\n";
                cout << "\t[-S <album-track sorting>] [-M <movie sorting>] [-A <PSP Drive Letter>]\n";
                cout << "\t[-V <debug verbosity>]\n\n";
                cout << "Output will set xml links to \n";
                cout << "\t\"<web file directory>/each audio file in<local file directory>\"\n\n";
                cout << "If specified, output will set image link to \n";
                cout << "\t\"<web file directory>/<channel image filename>\"\n";
                cout << "\tOtherwise the image link will be set to the first image found.\n\n";
                cout << "The time zone offset is in [+-]hhmm format (i.e., -0600)\n";
                cout << "\nAlbum-track sorting must have a value of either \"album\", \"track\",\n";
                cout << "\t\"both\", or \"none\".\n";
                cout << "Movie sorting must have a value of either \"file\", \"file_separate\",\n";
                cout << "\t\"channel\", \"channel_separate\", \"separate\", or \"none\". For \"file\",\n";
                cout << "\ta 2 will be appeneded beforethe extension.\n";
                cout << "\nFor a User Input Prompt, add -U as the only parameter.\n";
                cout << "For a version information, add --version as the only parameter.\n";
                return 0;
            }
            else
            {
                cout << "Incorrect parameter " << argv[i] << ". Ignoring.\n";
                i++;
            }
        }
    }

    //Debug
    printf("Done parsing input.\n");

    //Check for required input
    if (xmlfilename == "" || web_directory == "" || local_directory == "" || title == "")
    {
        cout << "Gimme a break here. I need input.\n";
        cout << "Some of PSP_RSS_feed assumes you are entering values correctly.\n\n";
        cout << "Unix/Linux:\n";
        cout << "Command prompt>PSP_RSS_feed [-U] -O <xml output file> -W <web file directory>\n";
        cout << "\t-L <local file directory> -T <channel title>\n";
        cout << "\t[-Z <time zone offset>] [-C <channel link>] [-D <channel description>]\n";
        cout << "\t[-R <channel copyright>] [-I <channel image filename>]\n";
        cout << "\t[-S <album-track sorting>] [-M <movie sorting>] [-A <Path to PSP>]\n";
        cout << "\t[-V <debug verbosity>]\n\n";
        cout << "DOS/Windows:\n";
        cout << "Command prompt>PSP_RSS_feed [-U] @inputfile\n";
        cout << "Where inputfile has one line containing:\n";
        cout << "\t-O <xml output file> -W <web file directory>\n";
        cout << "\t-L <local file directory> -T <channel title>\n";
        cout << "\t[-Z <time zone offset>] [-C <channel link>] [-D <channel description>]\n";
        cout << "\t[-R <channel copyright>] [-I <channel image filename>]\n";
        cout << "\t[-S <album-track sorting>] [-M <movie sorting>] [-A <PSP Drive Letter>]\n";
        cout << "\t[-V <debug verbosity>]\n\n";
        cout << "Output will set xml links to \n";
        cout << "\t\"<web file directory>/each audio file in<local file directory>\"\n\n";
        cout << "If specified, output will set image link to \n";
        cout << "\t\"<web file directory>/<channel image filename>\"\n";
        cout << "\tOtherwise the image link will be set to the first image found.\n\n";
        cout << "The time zone offset is in [+-]hhmm format (i.e., -0600)\n";
        cout << "\nAlbum-track sorting must have a value of either \"album\", \"track\",\n";
        cout << "\t\"both\", or \"none\".\n";
        cout << "Movie sorting must have a value of either \"file\", \"file_separate\",\n";
        cout << "\t\"channel\", \"channel_separate\", \"separate\", or \"none\". For \"file\",\n";
        cout << "\ta 2 will be appeneded beforethe extension.\n";
        cout << "\nFor a User Input Prompt, add -U as the only parameter.\n";
        cout << "For a version information, add --version as the only parameter.\n";
        return 1;
    }

//TODO: Add sanity checks to input

    //Calculate Video file name
    if (movie_order == "file" || movie_order == "file_separate")
    {
        //Get audio file name
        length = xmlfilename.length();

        //Get extension
        extension = xmlfilename.substr(xmlfilename.rfind('.'), length - xmlfilename.rfind('.'));

        //Remove extension
        audio = xmlfilename.substr(0, xmlfilename.rfind('.'));

        //Make Video file name
        videofilename = audio + "2" + extension;
    }

    //Check for image file
    if (image == "")
        get_image(local_directory, image);

    //Search directory for files
    error = get_audiolist(local_directory, audiolist, firmware_major, firmware_minor);
    if (error == 1)
    	return 1;

    //Replace Special Characters
    replace_special(special, web_directory);
    web_directory = special;
    replace_special(special, title);
    title = special;
    replace_special(special, link);
    link = special;
    replace_special(special, description);
    description = special;
    replace_special(special, copyright);
    copyright = special;
    replace_special(special, image);
    image = special;

    //Create xml file
    error = create_xml_file(xmlfile, xmlfilename, web_directory, image, title, link, description, copyright);
    if (error == 1)
    {
        return 1;
    }

    //Create video xml file
    if (movie_order == "file" || movie_order == "file_separate")
    {
        error = create_xml_file(videofile, videofilename, web_directory, image, title, link, description, copyright);
        if (error == 1)
        {
            return 1;
        }
    }

    //Set Date
    set_date(UTC_offset, date);

    //Sort by track or album
    order_audiolist(audiolist, local_directory, ordering);

    //Debug
    if (verbosity >= 4)
        printf("audiolist==>%s", audiolist.c_str());

    //Sort by type (audio, picture, or video)
    if (firmware_major > 2 || (firmware_major == 2 && firmware_minor >= 80))
    {
        order_movielist(audiolist, movie_order);
    }

    //Add values to xml file
    while(audiolist.length() > 6)
    {
        //Get audio file name
        length = audiolist.find('/');
        audio = audiolist.substr(0, length);

        //Check for compatibility
        extension = audio.substr(audio.rfind('.'), length - audio.rfind('.'));

        if (firmware_major > 2 || (firmware_major == 2 && firmware_minor >= 80))
        {
            if (extension == ".mp3" ||
                extension == ".m4a")
            {
                //Add audiofile
                add_audiofile(xmlfile, audiolist, local_directory, web_directory, date, false);
            }
            //Add video file
            else if (   extension == ".mp4" ||
                        extension == ".m4v" ||
                        extension == ".jpeg" ||
                        extension == ".jpg" ||
                        extension == ".gif" ||
                        extension == ".png" ||
                        extension == ".tiff" ||
                        extension == ".tif" ||
                        extension == ".bmp")
            {
                if (movie_order == "file" || movie_order == "file_separate")
                    add_audiofile(videofile, audiolist, local_directory, web_directory, date, true);
                else if (movie_order == "channel" || movie_order == "channel_separate")
                {
                    if (first_video)
                    {
                        //Start new channel
                        xmlfile << " </channel>\n";
                        xmlfile << " <channel>\n";
                        first_video = false;
                    }
                    add_audiofile(xmlfile, audiolist, local_directory, web_directory, date, true);
                }
                else
                    add_audiofile(xmlfile, audiolist, local_directory, web_directory, date, true);
            }
            else
            {
                //Drop file
                audiolist.replace(0, length+1, "");
                continue;
            }
        }
        else if (firmware_major == 2 && (firmware_minor < 80 && firmware_minor >= 70))
        {
            if (extension == ".mp3" ||
                extension == ".mp4" ||
                extension == ".m4a")
            {
                //Add audiofile
                add_audiofile(xmlfile, audiolist, local_directory, web_directory, date, false);
            }
            else
            {
                //Drop file
                audiolist.replace(0, length+1, "");
                continue;
            }
        }
        else if (firmware_major == 2 && firmware_minor < 70)
        {
            if (extension == ".mp3" ||
                extension == ".mp4")
            {
                //Add audiofile
                add_audiofile(xmlfile, audiolist, local_directory, web_directory, date, false);
            }
            else
            {
                //Drop file
                audiolist.replace(0, length+1, "");
                continue;
            }
        }
    }

    //close xml file
    close_xml_file(xmlfile);

    //close video file
    if (movie_order == "file" || movie_order == "file_separate")
        close_xml_file(videofile);

    //Prep for channel adding
    if (image == "")
        image = "/";

    //Add channel to PSP if path to PSP has been specified
    if (PSP_path != "")
    {
        add_rss(PSP_path, xmlfilename, web_directory, local_directory, image, title, description, copyright);

        //Add second file to PSP if firmware version supports it
        if (((firmware_major == 2 && firmware_minor > 80) || firmware_major > 2) && (movie_order == "file" || movie_order == "file_separate"))
            add_rss(PSP_path, videofilename, web_directory, local_directory, image, title, description, copyright);
    }

    return 0;
}
