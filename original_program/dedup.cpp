#include <stdlib.h>
#include <vector>
#include <iostream>
#include <iomanip>
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <map>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <stdlib.h>


//include the libraries needed for sha-1 hashing
#include <openssl/sha.h>

//include the libraries needed for the associative container storing
//hashed values. Note that this library is the current version of the old
//hash_map library. 
#include <unordered_map>

//include libraries for rabin's fingerprints
#include "rabinpoly.h"

//IMPORTANT!!!
//For the definitions below, if the minimum chunk size is set too low, large files can create
//far too many chunks. To avoid overflows, this value should be set sufficiently high enough
//to ensure that we aren't getting too many chunks, and thus, too many hashes
//We will enforce the creation of chunks between these values

//definitions for use with rabin's fingerprints
#define FINGERPRINT_PT		0xbfe6b8a5bf378d83LL	//a constant representation of a polynomial function
													//used to get the same results across the filesystem
#define BREAKMARK_VALUE		0x78					//a constant for determining which fingerprint
													//value determines a breakpoint
#define MIN_CHUNK_SIZE		8 //4096					//number of minimum bytes for a chunksize
#define MAX_CHUNK_SIZE		8192					//number of maximum bytes for a chunksize
#define WINDOW_SIZE			512						//Sets the size of the sliding window of the data
													//that creates a unique rabin fingerprint
#define BUFSIZE				4096					//Set the buffer size for the array that will hold
       												//the data read in from the file

using namespace std;

//static const unsigned chunk_size = 65536;
static const unsigned chunk_size = 2048;



int hit_max = 0;
int hit_break = 0;

////NOTE!!!!!!
//The only values above that should be changed are:
//MIN_CHUNK_SIZE		
//MAX_CHUNK_SIZE	
//WINDOW_SIZE
//chunk_size



struct filedata
{
	//The name of the file, as found in the filesystem
	string filename;
	//The size of the file, in bytes
	int size;
	//The extension the file has. If the file is not one of the most popular
	//extensions, or if the file has NO extension, then this string will be "". 
	string extension;
	//The full path of where the file is located in the system
	string filepath;
};

struct hashdata
{
	//The hash itself is saved as unsigned chars. The SHA-1 hash produces
	//a 20 byte hash.
	vector<unsigned char> hash;
	//The size of the file that was hashed, in bytes
	int size;
	//The extension of this hashed file
	string extension;
	//The string representation of the hash in a hexidecimal format
	string hex;
};


void add_to_vector(vector<vector<filedata> > &filesystem, string filename, int size, string path_name)
{
	filedata temp;
	temp.filename = filename;
	temp.size = size;
	temp.filepath = path_name;
	
	//Check the filename for a period "." Files with a period have extensions. 
	//unsigned find_index = filename.find(".");
	unsigned int find_index = filename.find_last_of(".");

	if(find_index > 10000)		//No period found
	{
		temp.extension = "";

		//If nothing is in the vector, just stick it anywhere
		if(filesystem.size() == 0)
		{
// *****************    debugging comment    *****************
//			cout << find_index << endl;
//			cout << filename << endl;

			vector<filedata> temp_vector;
			temp_vector.push_back(temp);
			filesystem.push_back(temp_vector);
		}
		//If something is in the vector, check the extension (in this case "") to see 
		//if it matches an already existing vector. If so, stick it there.
		//If not, create a new vector row. 
		else
		{
			int index = -1;

			for(int i = 0; i < filesystem.size(); i++)
				if(filesystem[i][0].extension == "")
					index = i;

			//No "" extension found
			if(index == -1)
			{
				vector<filedata> temp_vector;
				temp_vector.push_back(temp);
				filesystem.push_back(temp_vector);
			}
			//Extension found
			else
				filesystem[index].push_back(temp);
		}
		return;
	}
	else //Period/extension found
	{
// *****************    debugging comment    *****************
//		cout << find_index << endl;
//		cout << filename << endl;
		
        temp.extension = filename.substr(find_index, (filename.size() - find_index));

		//If nothing is in the vector, just stick it anywhere
		if(filesystem.size() == 0)
		{
			vector<filedata> temp_vector;
			temp_vector.push_back(temp);
			filesystem.push_back(temp_vector);
		}
		//If something is in the vector, check the extension to see 
		//if it matches an already existing vector. If so, stick it there.
		//If not, create a new vector row. 
		else
		{
			int index = -1;

			for(int i = 0; i < filesystem.size(); i++)
				if(filesystem[i][0].extension == temp.extension)
					index = i;

			//The extension not found
			if(index == -1)
			{
				vector<filedata> temp_vector;
				temp_vector.push_back(temp);
				filesystem.push_back(temp_vector);
			}
			//Extension found
			else
				filesystem[index].push_back(temp);
		}
		return;
	}
} // add_to_vector


void recursive_search(string start_directory, vector<vector<filedata> > &filesystem, int &num_files)
{
	//This vector holds the names of the files in the directory
	vector<string>file_names;

	//Required declarations for the directory pointer and directory stream
	DIR * directoryptr;
	struct dirent *direntptr;

	//Open the directory
	directoryptr = opendir(start_directory.c_str());

	//If the directory open failed, indicate to user
	if(directoryptr == NULL)
	{
		cerr << "Directory " << start_directory << "could not be opened.";
		return;
	}
	
	//While files exist in the directory, read their names and put them into the 
	//vector holding the file names (vector<string>file_names)
	while((direntptr = readdir(directoryptr)) != NULL)
	{
		string temp_name = (string(direntptr->d_name));
		
		//If the file is . or .., it is an up directory or self-directory, and 
		//can be disregarded
		if((temp_name == ".") || (temp_name == ".."))
			continue;
		else
		{
			string working_file;

			//Use stat to get file information from the system
			if(start_directory == "/")
				working_file = "/" +  temp_name;
			else
				working_file = start_directory + "/" +  temp_name;

// *****************    debugging comment    *****************
//			cout << working_file << endl;
		
			struct stat file_info;
			lstat(working_file.c_str(), &file_info);
			
			//Determine if the file is a symbolic link. If so, ignore it. 
			if(S_ISLNK(file_info.st_mode) == true)
				continue;
			//Ignore any file that is a pipe 
			else if(S_ISFIFO(file_info.st_mode) == true)
				continue;
			//Ignore any file that is a socket
			else if(S_ISSOCK(file_info.st_mode) == true)
				continue;
			//Determine if the file is a directory. If so, call recursive_search
			//in this directory. Disregard certain folders. 
			else if(S_ISDIR(file_info.st_mode) == true)
			{
				if(temp_name == "proc")
					continue;
				if(temp_name == "tmp")
					continue;
				if(temp_name == "project_files")
					continue;


				recursive_search(working_file, filesystem, num_files);
			}
			//If the file is not a directory, determine if it is a regular or
           //special file. Disregard special files. Add regular files to our vector.
			else if(S_ISREG(file_info.st_mode))
			{

				num_files++;

// *****************    debugging comment    *****************
//				cout << temp_name << endl;

				add_to_vector(filesystem, temp_name, file_info.st_size, working_file);
			}
			else
				continue;
		} // end else
	}// end while
	closedir(directoryptr);
} // recursive_search




void dedup_fingerprint(vector<vector<filedata> > &filesystem, vector<vector<hashdata> > &hashes, int &num_hashes)
{
	int num_extension_types = filesystem.size();
	vector<hashdata> temp;
	
	
	//1. Loop through all files in the system. 
	//2. If a file is equal or smaller than a window, we just hash
	//   hash the file, fingerprinting isn't applicable
	//3. For every other file, a window of 64 bytes is considered
	//4. The window moves byte-by-byte through the file, creating
	//   a rabin fingerprint checksum for this window
	//5. If the rabin fingerprint for the window value is equal
	//	 to a predetermined value, that window is a breakpoint
	//   This window is the last n bytes of this block of data
	//6. This data in the newly created block is hashed
	//7. Hashes are stored. If this was a full filesystem, if the hash
	//	 matched an existing hash, the data would be a duplicate, and
	//   only a reference to the original data would be required. Here
	//   we save all the hashes, then can later determine how many hashes
	//   are unique and how much data would be saved in a full implementation.

	//for debugging
	int filecount = 0;


	for(int i = 0; i < num_extension_types; i++)
	{
		for(int j = 0; j < filesystem[i].size(); j++)
		{
			filecount++;
			if(filecount % 1000 == 0)
				cout << filecount << " files done." << flush << endl;
// *****************    debugging comment    *****************
			//cout << "Hashing a file" << endl;
			//cout << filesystem[i][j].filepath << endl;

			//First check the file to see if it's size is 0. If it is, 
			//we can ignore it, as it won't have any dedup effects
			
			struct stat file_info;
			lstat(filesystem[i][j].filepath.c_str(), &file_info);
			if(file_info.st_size == 0)
				continue;
			
		
			//open the file being hashed, read-only, binary mode ("rb")
			FILE * fileptr = fopen(filesystem[i][j].filepath.c_str(), "rb");
			
			//If file opening fails, we can't do anything with this file due to
			//some sort of probem (permissions or otherwise)
			//Skip this file and go on to the next one
			if(fileptr == NULL)
			{
				continue;
			}

			//Check that the file size is greater than the minimum size we defined for 
			//fingerprint chunks. If the file is smaller than this minimum size, we won't
			//be making fingerprints out of it

			bool no_fingerprinting = false;
			if(file_info.st_size <= MIN_CHUNK_SIZE)
				no_fingerprinting = true;

			if(no_fingerprinting)
			{
				//debugging
				//cout << "NO FINGERPRINT FILE" << endl;

				SHA_CTX sha_struct;
				unsigned char* file_buffer;
				file_buffer = new unsigned char[16384];

				while(1)
				{
					int length;
					length = fread(file_buffer, 1, sizeof(file_buffer), fileptr);
					if(length == 0)
						break;
					SHA_Update(&sha_struct, file_buffer, length);
				}

				unsigned char* hash_output;
				hash_output = new unsigned char[20];

				//close the file
				int error_message = ferror(fileptr);
				fclose(fileptr);
				if(error_message)
				{
					continue;
				}
				//get the hash from SHA1, via hash_output
				SHA1_Final(hash_output, &sha_struct);

				//store the hash to our vector containing the hashes
				hashdata temp_struct;
				string tempstring;
				temp_struct.extension = filesystem[i][0].extension;
				temp_struct.size = file_info.st_size;
				for(int k = 0; k < 20; k++)
				{
					temp_struct.hash.push_back(hash_output[k]);
				}
				
				temp.push_back(temp_struct);
				
				//cleanup variables
				delete [] file_buffer;
				delete [] hash_output;
				num_hashes++;
			}
			//In the case fingerprinting is required, do so in the else branch
			else
			{
				//debugging
				//cout << "FINGERPRINT FILE" << flush << endl;

				//close the original file opening
				fclose(fileptr);

				//Initialize the fingerprinting class
				//Since breakpoints are determined using a polynomial (usually random), we
				//must use a constant to ensure that all breakpoints are constant across
				//the filesystem. 

				//debugging
				//cout << "INIT WINDOW FILE" << flush << endl;

				window rabinfp(FINGERPRINT_PT, WINDOW_SIZE);

				//Reset the window prior to every run, although probably not needed given
				//the locality of the class 
				rabinfp.reset();

				//Open the file using the standard open function to get a file descriptor
				//instead of a file pointer
				int fd = open(filesystem[i][j].filepath.c_str(), O_RDONLY);
				
				//if file fails to open, skip to the next file
				if (!fd)
				{
					continue;
				}

				//Do multiple reads on the file, putting the data into a buffer.
				//The data in the buffer will be used a byte at a time to determine
				//the breakpoints (chunks) in the file. Breakpoints will be saved
				//as integer values denoting how many bytes from the beginning of a 
				//file a breakpoint exists. These will be saved in a vector.

				int bytes_from_front = 0;
				int bytes_from_last_break = 0;
				vector<int> breakpoints;
				int count;
				char readbuffer[BUFSIZE];

				//debugging
				//cout << "GET BREAKPOINTS" << flush << endl;

				while((count = read(fd, readbuffer, BUFSIZE)) > 0)
				{
					//count now indicates how many bytes were read in by the read
					//operation. we will read in each byte for 'count' iterations

					u_int64_t fingerprint_value;

					for(int k = 0; k < count; k++)
					{
						//This function call returns the fingerprint value
						//as determined by rabin's fingerprinting
						fingerprint_value = rabinfp.slide8(readbuffer[k]);

						//if we have hit the maximum chunk size without finding a 
						//breakpoint using fingerprints, we make a breakpoint anyway
						#if 0
						if(bytes_from_last_break == MAX_CHUNK_SIZE)
						{
						  hit_max++;
							breakpoints.push_back(bytes_from_front);
							bytes_from_front++;
							bytes_from_last_break = 0;
						}
						#endif
						//Mod the fingerprint with the chunk size constant. If this result
						//equals our constant breakmark, then this could be a breakpoint
						//this was an else if
						
						if ((fingerprint_value % chunk_size) == BREAKMARK_VALUE)
						{

						  hit_break++;
							//if the number of bytes is between our range of acceptable
							//chunk sizes, we make it a breakpoint
						  #if 0
						  if((bytes_from_last_break >= MIN_CHUNK_SIZE) &&
								(bytes_from_last_break <= MAX_CHUNK_SIZE))
							{
						
						   #endif
							  breakpoints.push_back(bytes_from_front);
							  bytes_from_front++;
							  bytes_from_last_break = 0;
							 #if 0
							}
							else
							{
								bytes_from_front++;
								bytes_from_last_break++;
							}
						  #endif

						}
					       
						//This isn't a breakpoint. We've moved one byte, so we increment
						//the bytes from the beginning, and the bytes from the last breakpoint
						else
						{
							bytes_from_front++;
							bytes_from_last_break++;
						}
					}//end for
				}//end while
				//Clear out variables
				bytes_from_front = 0;
				bytes_from_last_break = 0;
				
				//cout << breakpoints.size() << " BREAKPOINTS" << flush << endl;

				//close file
				close(fd);

				//Now that we have the breakpoints, we know how many chunks are in the file, and
				//how much data needs to be read to hash. 
				int numchunks = breakpoints.size() + 1;

				//If the file fails to read at this point, something is wrong
				fileptr = fopen(filesystem[i][j].filepath.c_str(), "rb");
				if(fileptr == NULL)
				{
					cerr << "Error opening file after chunking. Critical. Terminating.";
					exit(1);
				}

				//debugging
				//cout << "READ FINGERPRINTS" << flush << endl;
				

				//Iterate through the chunks in a file, creating a hash for each chunk
				int previous_breakpoint = 0;
				for(int k = 0; k < numchunks; k++)
				{
					//cout << "READING CHUNK" << flush << endl;

					//In the case that this is the last run through the
					//file, the last breakpoint is the end of the file
					if(k == numchunks - 1)
					{
						previous_breakpoint = file_info.st_size;
					}

					unsigned char* hash_output;
					hash_output = new unsigned char[20];
					unsigned char* file_buffer;

					int chunksize_inbytes;
					if(breakpoints.size() == 0)
					{
						chunksize_inbytes = file_info.st_size;
					}
					else
					{
						chunksize_inbytes = breakpoints[k] - previous_breakpoint;
					}

					//In the case where the breakpoint is exactly at the end of the file, 
					//the previous breakpoint will be the file size. Handle this to avoid
					//attempting to allocate memory with size 0 or large negatives, as we
					//don't actually have another chunk for hashing in this instance
					if(chunksize_inbytes <= 0)
					{
						delete [] hash_output;
						continue;
					}
					
					//cout << "FILESIZE IS: " << file_info.st_size << flush << endl;
					//cout << "PREVIOUS BREAKPOINT WAS: " << previous_breakpoint << flush << endl;
					//cout << "CHUNKSIZE IS: " << chunksize_inbytes << flush << endl;

					//cout << "INIT NEW FILE BUFFER[" << chunksize_inbytes << "]" << flush << endl;
					file_buffer = new unsigned char[chunksize_inbytes];

					fread(file_buffer, chunksize_inbytes, 1, fileptr);

					//Create the struct to hold the data
					hashdata temp_struct;
					string tempstring;
					temp_struct.extension = filesystem[i][0].extension;
					temp_struct.size = chunksize_inbytes;

					//Get the SHA1 hash of the chunk
					SHA_CTX sha_struct;
					SHA1_Init(&sha_struct);
					SHA1_Update(&sha_struct, file_buffer, chunksize_inbytes);
					SHA1_Final(hash_output, &sha_struct);

					for(int l = 0; l < 20; l++)
						temp_struct.hash.push_back(hash_output[l]);

					temp.push_back(temp_struct);

					//cout << "DELETE MEM ALLOCATION" << flush << endl;
					//Cleanup memory
					delete [] hash_output;
					delete [] file_buffer;

					//Change the previous breakpoint to the current breakpoint
					if(breakpoints.size() > 0)
						previous_breakpoint = breakpoints[k];

					//Update number of hashes
					num_hashes++;
				}

				//close the file
				fclose(fileptr);

			}//end else(fingerprinting)
			
		}//end for "j"

		hashes.push_back(temp);
		temp.clear();
	}//end for "i"
}//end dedup_fingerprint




int main()
{
	int num_files = 0;
	int num_hashes = 0;
	unordered_map<string, string> dictionary;		//container for collision checking
	vector<vector<hashdata> > hashes;				//contains hashes, associated file data
	vector<vector<filedata> > filesystem;			//contains system file data
	
	struct results									//stores results of deduplication
	{
		string extension;
		int total_hashes;
		int num_hash_collisions;
		int bytes_saved;
	};

	vector<results> dedup_results;					//stores all individual file results info
	results systemwide_results;						//stores systemwide dedup data
	systemwide_results.bytes_saved = 0;
	systemwide_results.num_hash_collisions = 0;

	//Get the files in the system
	recursive_search("/home/kwoelfe/dedup/test_files", filesystem, num_files);

	cout <<"Please be patient. Fingerprinting the entire filesystem takes about " << flush << endl;
	cout << "30 minutes on a ~6 GB system." << flush << endl << endl;

	//Create file chunks, create hashes from the chunks
	dedup_fingerprint(filesystem, hashes, num_hashes);

	cout <<"Fingerprinting and hashing is completed." << flush << endl;
	cout <<"Please wait while hashes are checked for collisions and results stored." << flush << endl << endl;

	//Convert the hashes into hex format, for container entries
	//Insert hashes into unordered map container to find collisions
	//Save results
	char hexes[41];
	int num_hashes_hexes = 0;
	int status = 0;
	results temp;

	for(int i=0; i < hashes.size(); i++)
	{
		//Initialize the results struct
		temp.extension = "";
		temp.total_hashes = 0;
		temp.bytes_saved = 0;
		temp.num_hash_collisions = 0;

		for(int j = 0; j < hashes[i].size(); j++)
		{
			for(int k = 0; k < 20; k++)
				sprintf(&hexes[k*2], "%02X", hashes[i][j].hash[k]);

			hashes[i][j].hex = hexes;
			num_hashes_hexes++;

			//Now add data to results
			pair<string, string> hash_entries (hashes[i][j].hex, hashes[i][j].extension);
			pair<unordered_map<string,string>::const_iterator, bool> return_value = 
				dictionary.insert(hash_entries);

			if(return_value.second == false)
			{
				temp.extension = hashes[i][j].extension;
				temp.bytes_saved = temp.bytes_saved + hashes[i][j].size;
				temp.num_hash_collisions++;
			}

			if(temp.num_hash_collisions != 0 && j == hashes[i].size() - 1)
			{
				temp.total_hashes = hashes[i].size();
				dedup_results.push_back(temp);
			}
		}
	}

	cout << "Extension, Total Hashes, Number of Hash Collisions, Bytes Saved " << endl;

	for(int i = 0; i < dedup_results.size(); i++)
	{
		systemwide_results.bytes_saved = (systemwide_results.bytes_saved + 1);
			dedup_results[i].bytes_saved;

		systemwide_results.num_hash_collisions = systemwide_results.num_hash_collisions +
			dedup_results[i].num_hash_collisions;

// *****************    debugging comment    *****************
		cout << dedup_results[i].extension << ", ";
		cout << dedup_results[i].total_hashes << ", ";
		cout << dedup_results[i].num_hash_collisions << ", ";
		cout << dedup_results[i].bytes_saved << endl;
		
	}

	cout << "Files Scanned, " << num_files << endl;
    cout << "Hashes Stored, " << num_hashes << endl;
	cout << "Hash Collisions, " << systemwide_results.num_hash_collisions << endl;
	cout << "Bytes Saved, " << systemwide_results.bytes_saved << endl;

	cout << "Max chunk: " << hit_max << endl;
	cout << "Breaks found: " << hit_break << endl;
	

	return 0;
}

