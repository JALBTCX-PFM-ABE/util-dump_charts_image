
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


/*********************************************************************************************

    This program is public domain software that was developed by 
    the U.S. Naval Oceanographic Office.

    This is a work of the US Government. In accordance with 17 USC 105,
    copyright protection is not available for any work of the US Government.

    This software is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

*********************************************************************************************/

#ifndef VERSION

#define     VERSION     "PFM Software - pfm_charts_image V2.16 - 07/23/14"

#endif

/*

    Version 1.0
    Jan C. Depner
    10/26/04

    First working version.


    Version 1.01
    Jan C. Depner
    11/05/04

    Changed sign on heading for ERDAS, output R,P instead of P,R.


    Version 1.02
    Jan C. Depner
    12/16/04

    Changed Usage message for PFM 4.5 directory input.


    Version 1.03
    Jan C. Depner
    01/14/05

    Output DEM in UTM coordinates.


    Version 1.04
    Jan C. Depner
    02/25/05

    Switched to open_existing_pfm_file from open_pfm_file.


    Version 1.1
    Jan C. Depner
    03/14/05

    Now handles HOF files for those rare occasions when you go over
    land but don't have TOF files.


    Version 2.0
    Jan C. Depner
    08/09/05

    Hopefully we now have everything right for ERDAS to do the photo mosaicing.


    Version 2.01
    Jan C. Depner
    08/12/05

    Added -n option to override limits on too dark or light pictures.


    Version 2.02
    Jan C. Depner
    10/12/05

    Added -m option to decimate the number of pictures output (Every mth picture).


    Version 2.03
    Jan C. Depner
    10/26/05

    Changed usage for PFM 4.6 handle file use.


    Version 2.04
    Jan C. Depner
    01/03/06

    Minor change to image_read_record call.


    Version 2.05
    Jan C. Depner
    06/05/06

    Removed inside.c, get_area_mbr.c, ellpfns.c, projfns.c, and tmfns.c.  Moved to utility.


    Version 2.1
    Jan C. Depner
    07/31/06

    Added pos time offset option.  Changed name of pos.txt output file to pos.dat.  Added area file name to
    beginning of pos.dat and dem.dat (areaname_pos.dat).  Added flip sign option.  Fixed the omega, phi, kappa
    computations and the interpolation in the charts pos_io function...  DOH!!!


    Version 2.11
    Jan C. Depner
    01/23/07

    Check for upper case SBET file name to correct for GCS screwup.


    Version 2.12
    Jan C. Depner
    10/22/07

    Added fflush calls after prints to stderr since flush is not automatic in Windows.


    Version 2.13
    Jan C. Depner
    04/07/08

    Replaced single .h files from utility library with include of nvutility.h


    Version 2.14
    Jan C. Depner
    05/06/11

    Fixed problem with getopt that only happens on Windows.


    Version 2.15
    Jan C. Depner (PFM Software)
    02/26/14

    Cleaned up "Set but not used" variables that show up using the 4.8.2 version of gcc.


    Version 2.16
    Jan C. Depner (PFM Software)
    07/23/14

    - Switched from using the old NV_INT64 and NV_U_INT32 type definitions to the C99 standard stdint.h and
      inttypes.h sized data types (e.g. int64_t and uint32_t).

*/
