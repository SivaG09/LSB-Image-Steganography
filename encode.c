
//declaring the required header files

#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"
/* Function Definitions */

//function defintion to read and validate encode args
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{   
	//checking if 5th CLA equal to NULL or not
	if(argv[4] == NULL)
	{
		printf("INFO : Output File not mentioned. Creating steged_img.bmp as default\n");
	}
	//compare and validating the bmp files
	if( strcmp(strstr(argv[2],".bmp"),".bmp") == 0 )
	{
		printf("INFO : Opening required files\n");
		printf("INFO : Opened %s \n", argv[2]);
		//storing the bmp file to src image file
		encInfo->src_image_fname = argv[2];
	}
	else
	{
		return e_failure;
	}
	//compare and validating the text file
	if( strcmp(strstr(argv[3],".txt"),".txt") == 0 )
	{
		printf("INFO : Opened %s \n",argv[3]);
		//storing the txt file to secret file
		encInfo->secret_fname = argv[3];
	}
	else
	{
		return e_failure;
	}
	if( argv[4] != NULL )
	{
		encInfo->stego_image_fname = argv[4];
		printf("INFO : Opened %s \n",argv[4]);
	}
	else
	{
		encInfo->stego_image_fname = "steged_img.bmp";
        printf("INFO : Opened steged_img.bmp\n");		
	}
	return e_success;
}

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("INFO : width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("INFO : height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

//function defintion for get_file_size
uint get_file_size(FILE *fptr)
{
	fseek(fptr,0,SEEK_END);
	return ftell(fptr);
}

//function defintion for checking capacity
Status check_capacity(EncodeInfo *encInfo)
{
	encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
	encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
	//checking image capacity is greater than required size
    if( encInfo -> image_capacity > 16+32+32+encInfo->size_secret_file*8 )
	{
		return e_success;
	}	
	else
	{
		return e_failure;
	}
}

//function defintion for copying bmp header
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
  char str[54];
  fseek(fptr_src_image,0,SEEK_SET);
  fread(str,54,1,fptr_src_image);
  fwrite(str,54,1,fptr_dest_image);
  return e_success;  
}

//function defintion for encode byte to lsb
Status encode_byte_to_lsb(char data, char *image_buffer)
{
 for ( int i = 0; i < 8; i++ )
 {
	 image_buffer[i] = (image_buffer[i] & 0xFE ) | ((data >> i) & i);
 }
}

//function defintion for encode data to image
Status encode_data_to_image(char *data, int size, EncodeInfo *encInfo)
{
	for ( int i = 0; i < size; i++ )
	{
		fread(encInfo->image_data,8,1,encInfo->fptr_src_image);
		encode_byte_to_lsb(data[i], encInfo->image_data);
		fwrite(encInfo->image_data,8,1,encInfo->fptr_stego_image);
	}
}

//function defintion for encoding magic string
Status encode_magic_string( char *magic_string, EncodeInfo *encInfo)
{
	//calling function encode data to image
	encode_data_to_image(magic_string,strlen(magic_string),encInfo);
	return e_success;
}

//function defintion for encode size to lsb
Status encode_size_to_lsb(int size, EncodeInfo *encInfo)
{
	char str[32];
	fread (str,32,1,encInfo->fptr_src_image);
	for(int i = 0; i < 32; i++)
	{
		str[i] = (str[i] & 0xFE) | ((size >> i) & 1);
	}
	fwrite(str,32,1,encInfo->fptr_stego_image);
	return e_success;
}

//function defintion for encode secret file extn size
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
	encode_size_to_lsb(size,encInfo);
	return e_success;
}

//function defintion for encode secret file extn
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
	encode_data_to_image(file_extn,strlen(file_extn),encInfo);
	return e_success;
}

//function defintion for encode secret file size
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
	encode_size_to_lsb(file_size,encInfo);
	return e_success;
}

//function definition to encode secret file data
Status encode_secret_file_data(EncodeInfo *encInfo)
{
	char str[encInfo->size_secret_file];
	fseek(encInfo->fptr_secret,0,SEEK_SET);
	fread(str,encInfo->size_secret_file,1,encInfo->fptr_secret);
	encode_data_to_image(str,encInfo->size_secret_file,encInfo);
	return e_success;
}

//function definition for copying remaining data
Status copy_remaining_img_data(EncodeInfo *encInfo)
{
	int len = (encInfo->image_capacity + 54) - ftell(encInfo->fptr_stego_image);
	char str[len];
	fread(str,len,1,encInfo->fptr_src_image);
	fwrite(str,len,1,encInfo->fptr_stego_image);
	return e_success;
}

//function defintion for do encoding
Status do_encoding(EncodeInfo *encInfo)
{   
	printf("INFO : ## Encoding Procedure Satrted ##\n");
	//function call for opening decoding files and checking it is success or not
	if ( open_files(encInfo) == e_success )
	{
	//function call for opening decoding files and checking it is success or not
		if ( check_capacity(encInfo) == e_success )
		{
			printf("INFO : Checking for beautiful.bmp capacity to handle secret.txt\n");
			printf("INFO : Done. Found OK\n");
	        //function call for copy bmp header and checking it is success or not
			if ( copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
			{
				printf("INFO : Copying Image Header\n");
				printf("INFO : Done\n");
	            //function call for encode magic string and checking it is success or not
				if( encode_magic_string(MAGIC_STRING,encInfo) == e_success )
				{
					printf("INFO : Encoding Magic String Signature\n");
					printf("INFO : Done\n");
					strcpy(encInfo->extn_secret_file ,strstr(encInfo->secret_fname,"."));
	                //function call for encode secret file extn size and checking it is success or not
					if( encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo) == e_success )
					{
						printf("INFO : Encoding secret File Extension Size\n");
						printf("INFO : Done\n");
	                    //function call for encode secret file extn and checking it is success or not
						if( encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
						{
							printf("INFO : Encoding secret.txt File Extension\n");
							printf("INFO : Done\n");
	                        //function call for encode secret file size and checking it is success or not
							if(encode_secret_file_size(encInfo->size_secret_file,encInfo) == e_success)
							{
								printf("INFO : Encoding secret.txt File Size\n");
                                printf("INFO : Done\n");
	                            //function call for encode secret file data and checking it is success or not
								if( encode_secret_file_data(encInfo) == e_success)
								{
									printf("INFO : Encoding secret.txt File Data\n");
									printf("INFO : Done\n");
	                                //function call for copying remaining img data and checking it is success or not
									if( copy_remaining_img_data(encInfo) == e_success)
     								{
										printf("INFO : Copying left Over Data\n");
										printf("INFO : Done\n");
									}
									else
									{
										//printing error message and returning failure
										printf("ERROR : Copying left over data failed\n");
										return e_failure;
									}
								}
								else
								{
						            //printing error message and returning failure
									printf("ERROR : Encoding Secret file failed\n");
									return e_failure;
								}
							}
							else
							{
						        //printing error message and returning failure
								printf("ERROR : Encoding Secret file size failed\n");
								return e_failure;
							}
						}
						else
						{
						    //printing error message and returning failure
							printf("ERROR : Encoding secret file extension failed\n");
							return e_failure;
						}
					}
					else
					{
						//printing error message and returning failure
						printf("ERROR : Encoding secret file extension size failed\n");
						return e_failure;
					}
				}
				else
				{
					//printing error message and returning failure
					printf("ERROR : Encoding magic string failed\n");
					return e_failure;
				}
			}
			else
			{
						//printing error message and returning failure
				printf("ERROR : BMP header not copied successfully\n");
				return e_failure;
			}
		}
		else
		{
			//printing error message and returning failure
			printf("ERROR : Check capacity is failure\n");
			return e_failure;
		}
	}
	else
	{
		//printing error message and returning failure
		printf("ERROR : Opening files is Failure");
		return e_failure;
	}
	return e_success;
}
