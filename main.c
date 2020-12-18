
/*********************************************************************************************

    This is public domain software that was developed by or for the U.S. Naval Oceanographic
    Office and/or the U.S. Army Corps of Engineers.

    This is a work of the U.S. Government. In accordance with 17 USC 105, copyright protection
    is not available for any work of the U.S. Government.

    Neither the United States Government, nor any employees of the United States Government,
    nor the author, makes any warranty, express or implied, without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, or assumes any liability or
    responsibility for the accuracy, completeness, or usefulness of any information,
    apparatus, product, or process disclosed, or represents that its use would not infringe
    privately-owned rights. Reference herein to any specific commercial products, process,
    or service by trade name, trademark, manufacturer, or otherwise, does not necessarily
    constitute or imply its endorsement, recommendation, or favoring by the United States
    Government. The views and opinions of authors expressed herein do not necessarily state
    or reflect those of the United States Government, and shall not be used for advertising
    or product endorsement purposes.

*********************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>


#include "FilePOSOutput.h"
#include "FileImage.h"

#include "nvutility.h"

#include "version.h"


/*****************************************************************************

    Program:    dump_charts_image

    Purpose:    Extracts .jpg image files from a CHARTS .img file, gets plane
                attitude/position from an associated .sbet or .pos file, and
                dumps the .jpg files to a directory.

    Programmer: Jan C. Depner

    Date:       09/29/06

*****************************************************************************/


void usage ()
{
  fprintf (stderr, "\nUsage: dump_charts_image -r ROLL_BIAS -p PITCH_BIAS -h HEADING_BIAS [-n] [-i INTERVAL] [-t TIME_OFFSET] <IMAGE_FILE> <OUTPUT_DIR>\n");
  fprintf (stderr, "\nWhere:\n\n");
  fprintf (stderr, 
           "\t-r ROLL_BIAS = camera_boresight_roll: value from [DIGITAL_CAMERA_SECTION] of mission planning text file\n");
  fprintf (stderr, 
           "\t-p PITCH_BIAS = camera_boresight_pitch: value from [DIGITAL_CAMERA_SECTION] of mission planning text file\n");
  fprintf (stderr, 
           "\t-h HEADING_BIAS = camera_boresight_heading: value from [DIGITAL_CAMERA_SECTION] of mission planning text file\n");
  fprintf (stderr, "\t-n = do not remove too light or too dark pictures.\n");
  fprintf (stderr, "\t-i = only output pictures at INTERVAL spacing (-i 2 = output every other picture)\n");
  fprintf (stderr, "\t-t = add the TIME_OFFSET (in seconds) to the time of the image before retrieving the attitude\n\n");
  fprintf (stderr, "\tIMAGE_FILE = CHARTS .img file name.\n");
  fprintf (stderr, "\tOUTPUT_DIR = Output directory name (if it doesn't exist it will be created).\n\n");
  fflush (stderr);
}



int32_t main (int32_t argc, char **argv)
{
  FILE                *img_fp, *pos_fp, *jpg_fp;
  float               roll_bias, pitch_bias, heading_bias, time_offset = 0.0;
  int32_t             i, percent = 0, old_percent = -1, icount = 0, interval = 1, year, jday, hour, minute, int_count = 0;
  float               second, heading;
  char                dir[512], img_file[512], pos_file[512], temp[512], jpg_file[512];
  uint8_t             *image;
  IMAGE_INDEX_T       image_index;
  IMAGE_HEADER_T      image_head;
  char                c;
  extern char         *optarg;
  extern int          optind;
  uint8_t             roll_input = NVFalse, pitch_input = NVFalse, heading_input = NVFalse,
                      no_limit = NVFalse;
  POS_OUTPUT_T        pos;
  int64_t             new_stamp, image_time;
  uint32_t            size;
  double              lat_degs, lon_degs;


  printf ("\n\n %s \n\n\n", VERSION);


  while ((c = getopt (argc, argv, "r:p:h:d:ni:t:f")) != EOF)
    {
      switch (c)
        {
        case 'r':
          sscanf (optarg, "%f", &roll_bias);
          roll_input = NVTrue;
          break;

        case 'p':
          sscanf (optarg, "%f", &pitch_bias);
          pitch_input = NVTrue;
          break;

        case 'h':
          sscanf (optarg, "%f", &heading_bias);
          heading_input = NVTrue;
          break;

        case 'i':
          sscanf (optarg, "%d", &interval);
          break;

        case 't':
          sscanf (optarg, "%f", &time_offset);
          break;

        case 'n':
          no_limit = NVTrue;
          break;

        default:
          usage ();
          exit (-1);
          break;
        }
    }


  if (optind >= argc || !roll_input || !pitch_input ||!heading_input)
    {
      usage ();
      exit (-1);
    }


  strcpy (img_file, argv[optind]);
  strcpy (dir, argv[optind + 1]);
 

#ifdef NVWIN3X
 #ifdef __MINGW64__
  if (mkdir (dir))
 #else
  if (_mkdir (dir))
 #endif
#else
  if (mkdir (dir, 00777))
#endif
    {
      if (errno != 17)
        {
          perror (dir);
          exit (-1);
        }
    }


  fprintf (stderr, "Extracting images\n\n");
  fflush (stderr);


  if ((img_fp = open_image_file (img_file)) == NULL) 
    {
      perror (img_file);
      exit (-1);
    }

  image_read_header (img_fp, &image_head);


  for (i = 1 ; i <= image_head.text.number_images ; i++)
    {
      if (image_get_metadata (img_fp, i, &image_index)) break;


      /*  Find, open, and read the sbet (reprocessed) or pos file information.  */

      if (!get_pos_file (img_file, pos_file)) 
        {
          fprintf (stderr, "Unable to find SBET or POS file.\n");
          exit (-1);
        }

      if ((pos_fp = open_pos_file (pos_file)) == NULL)
        {
          perror (pos_file);
          exit (-1);
        }


      /*  Get the attitude data for this image.  */

      new_stamp = pos_find_record (pos_fp, &pos, image_index.timestamp + (time_offset * 1000000));

      if (!new_stamp) 
        {
          fclose (pos_fp);
          continue;
        }
      fclose (pos_fp);


      lat_degs = pos.latitude * NV_RAD_TO_DEG;
      lon_degs = pos.longitude * NV_RAD_TO_DEG;


      /*  Read the actual image.  */

      image = image_read_record (img_fp, image_index.timestamp, &size, &image_time);


      /*  If the size is less than 150000 this image is either too bright or too dark.  */

      if (no_limit || size >= 150000)
        {
          heading = fmod ((pos.platform_heading - pos.wander_angle) * NV_RAD_TO_DEG, 360.0);
          if (heading < -180.0) heading += 360.0;
          if (heading > 180.0) heading -=360.0;


          if (!(int_count % interval))
            {

              /*  Open the jpg file.  */


              /*  Convert the image time to something reasonable  */

              charts_cvtime (image_time, &year, &jday, &hour, &minute, &second);


              /*  Set the .jpg file name.  */

              sprintf (jpg_file, "%s%1cP%04d%03d%02d%02d%05.2f__%02.7f__%03.7f.jpg", dir, (char) SEPARATOR, year + 1900, jday, hour,
                       minute, second, lat_degs, lon_degs);


              if ((jpg_fp = fopen (jpg_file, "wb")) == NULL) 
                {
                  perror (jpg_file);
                  free (image);
                  exit (-1);
                }


              /*  Write the image to the jpg file.  */

              fwrite (image, size, 1, jpg_fp);

              fclose (jpg_fp);

              free (image);


              sprintf (temp, "mogrify -rotate %f -comment \"Lat: %f  Lon: %f  Heading: %f\" %s", heading, lat_degs,
                       lon_degs, heading, jpg_file);

              if (system (temp))
                {
                  fprintf (stderr, "Error running command:\n%s\n", temp);
                  exit (-1);
                }

              icount++;
            }
          int_count++;
        }

      percent = ((float) i / (float) image_head.text.number_images) * 100.0;
      if (percent != old_percent)
        {
          old_percent = percent;
          fprintf (stderr, "%03d%% processed            \r", percent);
          fflush (stderr);
        }
    }


  fclose (img_fp);


  fprintf (stderr, "Extracted %d images       \n\n", icount);
  fflush (stderr);


  return (0);
}
