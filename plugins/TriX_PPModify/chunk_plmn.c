#ifndef __TRIX_CHUNK_PLMN_C__
#define __TRIX_CHUNK_PLMN_C__

#include "defines.h"
#include "seer.h"
#include "segment.h"
#include "stage.h"
#include "file_io.h"
#include "file.h"
#include "workspace.h"
#include "util.h"
#include "options.h"
#include "mem.h"
#include "treenode.h"
#include "trixplug.h"
#include "ppmodify.h"

extern int dump_chunks;
extern int endianess_be;
extern struct trix_functions *ft;

#include "trixplug_wrapper.h"


unsigned int mnc_version = 0; // 0 - dct3, 1 - newer phones

typedef struct {
   unsigned int length;
   unsigned int mnc;
   unsigned int extend;   // dct3 only

   unsigned int namelen;
   char name[256];
} plmn_mnc;

// the PLMN subchunk
typedef struct {
   unsigned int cntr;      // should be 'CNTR'
   unsigned int length;
   unsigned int mcc;
   unsigned int count;     // number of MNC in this subchunk
   unsigned int flags;
   char country[5];
} plmn_hdr;

//-------------------------------------------------------------------------------------------------

unsigned int bcd2int ( t_workspace *ws, unsigned int pos, unsigned int length )
{
   unsigned int ret = 0;
   unsigned int lo = 0;
   unsigned int hi = 0;

   while ( length )
   {
      lo = v_get_b ( ws, pos ) & 0xF;
      hi = v_get_b ( ws, pos ) >> 4;

      if ( lo == 0xF )
         return ret;
      else
         ret = ret * 10 + lo;

      if ( hi == 0xF )
         return ret;
      else
         ret = ret * 10 + hi;

      pos++;
      length--;
   }

   return ret;
}
// TODO: rewrite that crappy code
unsigned int int2bcd ( unsigned int val, unsigned int length )
{
   if ( val < 10 && length > 1 )
      val = 0xF0 + val;
   else if ( val < 100 )
      val = ( val % 10 ) * 0x10 + ( val / 10 );
   else
      return 0xFF;

   return val;
}

// hmm, it is really needed?
const char *mcc2name ( unsigned int mcc )
{
   switch ( mcc )
   {
      case 202:
         return "Greece";

      case 204:
         return "Netherlands";

      case 206:
         return "Belgium";

      case 208:
         return "France";

      case 212:
         return "Monaco";

      case 213:
         return "Andorra";

      case 214:
         return "Spain";

      case 216:
         return "Hungary";

      case 218:
         return "Bosnia and Herzegovina";

      case 219:
         return "Croatia";

      case 220:
         return "Serbia";

      case 222:
         return "Italy";

      case 225:
         return "Vatican";

      case 226:
         return "Romania";

      case 228:
         return "Switzerland";

      case 230:
         return "Czech Republic";

      case 231:
         return "Slovakia";

      case 232:
         return "Austria";

      case 234:
      case 235:
         return "United Kingdom";

      case 238:
         return "Denmark";

      case 240:
         return "Sweden";

      case 242:
         return "Norway";

      case 244:
         return "Finland";

      case 246:
         return "Lithuania";

      case 247:
         return "Latvia";

      case 248:
         return "Estonia";

      case 250:
         return "Russia";

      case 255:
         return "Ukraine";

      case 257:
         return "Belarus";

      case 259:
         return "Moldova";

      case 260:
         return "Poland";

      case 262:
         return "Germany";

      case 266:
         return "Gibraltar";

      case 268:
         return "Portugal";

      case 270:
         return "Luxembourg";

      case 272:
         return "Ireland";

      case 274:
         return "Iceland";

      case 276:
         return "Albania";

      case 278:
         return "Malta";

      case 280:
         return "Cyprus";

      case 282:
         return "Georgia";

      case 283:
         return "Armenia";

      case 284:
         return "Bulgaria";

      case 286:
         return "Turkey";

      case 288:
         return "Faroe Islands";

      case 289:
         return "Abkhazia";

      case 290:
         return "Greenland";

      case 292:
         return "San Marino";

      case 293:
         return "Slovenia";

      case 294:
         return "Macedonia";

      case 295:
         return "Liechtenstein";

      case 297:
         return "Montenegro";

      case 302:
         return "Canada";

      case 308:
         return "Saint Pierre and Miquelon";

      case 310:
      case 311:
      case 312:
      case 313:
      case 314:
      case 315:
      case 316:
         return "USA";

      case 330:
         return "Puerto Rico";

      case 332:
         return "U.S. Virgin Islands";

      case 334:
         return "Mexico";

      case 338:
         return "Jamaica";

      case 340:
         return "Guadeloupe";

      case 342:
         return "Barbados";

      case 344:
         return "Antigua and Barbuda";

      case 346:
         return "Cayman Islands";

      case 348:
         return "British Virgin Islands";

      case 350:
         return "Bermuda";

      case 352:
         return "Grenada";

      case 354:
         return "Montserrat";

      case 356:
         return "Saint Kitts and Nevis";

      case 358:
         return "Saint Lucia";

      case 360:
         return "Saint Vincent and the Grenadines";

      case 362:
         return "Netherlands Antilles";

      case 363:
         return "Aruba";

      case 364:
         return "Bahamas";

      case 365:
         return "Anguilla";

      case 366:
         return "Dominica";

      case 368:
         return "Cuba";

      case 370:
         return "Dominican Republic";

      case 372:
         return "Haiti";

      case 374:
         return "Trinidad and Tobago";

      case 376:
         return "Turks and Caicos Islands";

      case 400:
         return "Azerbaijan";

      case 401:
         return "Kazakhstan";

      case 402:
         return "Bhutan";

      case 404:
      case 405:
         return "India";

      case 410:
         return "Pakistan";

      case 412:
         return "Afghanistan";

      case 413:
         return "Sri Lanka";

      case 414:
         return "Myanmar";

      case 415:
         return "Lebanon";

      case 416:
         return "Jordan";

      case 417:
         return "Syria";

      case 418:
         return "Iraq";

      case 419:
         return "Kuwait";

      case 420:
         return "Saudi Arabia";

      case 421:
         return "Yemen";

      case 422:
         return "Oman";

      case 423:
         return "Palestine";

      case 424:
      case 430:
      case 431:
         return "United Arab Emirates";

      case 425:
         return "Israel";

      case 426:
         return "Bahrain";

      case 427:
         return "Qatar";

      case 428:
         return "Mongolia";

      case 429:
         return "Nepal";

      case 432:
         return "Iran";

      case 434:
         return "Uzbekistan";

      case 436:
         return "Tajikistan";

      case 437:
         return "Kyrgyzstan";

      case 438:
         return "Turkmenistan";

      case 440:
      case 441:
         return "Japan";

      case 450:
         return "South Korea";

      case 452:
         return "Viet Nam";

      case 454:
         return "Hong Kong";

      case 455:
         return "Macau";

      case 456:
         return "Cambodia";

      case 457:
         return "Laos";

      case 460:
         return "China";

      case 466:
         return "Taiwan";

      case 467:
         return "North Korea";

      case 470:
         return "Bangladesh";

      case 472:
         return "Maldives";

      case 502:
         return "Malaysia";

      case 505:
         return "Australia";

      case 510:
         return "Indonesia";

      case 514:
         return "East Timor";

      case 515:
         return "Philippines";

      case 520:
         return "Thailand";

      case 525:
         return "Singapore";

      case 528:
         return "Brunei Darussalam";

      case 530:
         return "New Zealand";

      case 534:
         return "Northern Mariana Islands";

      case 535:
         return "Guam";

      case 536:
         return "Nauru";

      case 537:
         return "Papua New Guinea";

      case 539:
         return "Tonga";

      case 540:
         return "Solomon Islands";

      case 541:
         return "Vanuatu";

      case 542:
         return "Fiji";

      case 543:
         return "Wallis and Futuna";

      case 544:
         return "American Samoa";

      case 545:
         return "Kiribati";

      case 546:
         return "New Caledonia";

      case 547:
         return "French Polynesia";

      case 548:
         return "Cook Islands";

      case 549:
         return "Samoa";

      case 550:
         return "Federated States of Micronesia";

      case 551:
         return "Marshall Islands";

      case 552:
         return "Palau";

      case 602:
         return "Egypt";

      case 603:
         return "Algeria";

      case 604:
         return "Morocco";

      case 605:
         return "Tunisia";

      case 606:
         return "Libya";

      case 607:
         return "Gambia";

      case 608:
         return "Senegal";

      case 609:
         return "Mauritania";

      case 610:
         return "Mali";

      case 611:
         return "Guinea";

      case 612:
         return "Ivory Coast";

      case 613:
         return "Burkina Faso";

      case 614:
         return "Niger";

      case 615:
         return "Togo";

      case 616:
         return "Benin";

      case 617:
         return "Mauritius";

      case 618:
         return "Liberia";

      case 619:
         return "Sierra Leone";

      case 620:
         return "Ghana";

      case 621:
         return "Nigeria";

      case 622:
         return "Chad";

      case 623:
         return "Central African Republic";

      case 624:
         return "Cameroon";

      case 625:
         return "Cape Verde";

      case 626:
         return "Sao Tomé and Príncipe";

      case 627:
         return "Equatorial Guinea";

      case 628:
         return "Gabonese Republic";

      case 629:
         return "Republic of the Congo";

      case 630:
         return "Democratic Republic of the Congo";

      case 631:
         return "Angola";

      case 632:
         return "Guinea-Bissau";

      case 633:
         return "Seychelles";

      case 634:
         return "Sudan";

      case 635:
         return "Rwanda";

      case 636:
         return "Ethiopia";

      case 637:
         return "Somalia";

      case 638:
         return "Djibouti";

      case 639:
         return "Kenya";

      case 640:
         return "Tanzania";

      case 641:
         return "Uganda";

      case 642:
         return "Burundi";

      case 643:
         return "Mozambique";

      case 645:
         return "Zambia";

      case 646:
         return "Madagascar";

      case 647:
         return "Réunion";

      case 648:
         return "Zimbabwe";

      case 649:
         return "Namibia";

      case 650:
         return "Malawi";

      case 651:
         return "Lesotho";

      case 652:
         return "Botswana";

      case 653:
         return "Swaziland";

      case 654:
         return "Comoros";

      case 655:
         return "South Africa";

      case 657:
         return "Eritrea";

      case 702:
         return "Belize";

      case 704:
         return "Guatemala";

      case 706:
         return "El Salvador";

      case 708:
         return "Honduras";

      case 710:
         return "Nicaragua";

      case 712:
         return "Costa Rica";

      case 714:
         return "Panama";

      case 716:
         return "Peru";

      case 722:
         return "Argentina";

      case 724:
         return "Brazil";

      case 730:
         return "Chile";

      case 732:
         return "Colombia";

      case 734:
         return "Venezuela";

      case 736:
         return "Bolivia";

      case 738:
         return "Guyana";

      case 740:
         return "Ecuador";

      case 742:
         return "French Guiana";

      case 744:
         return "Paraguay";

      case 746:
         return "Suriname";

      case 748:
         return "Uruguay";

      case 750:
         return "Falkland Islands";

      default:
         return "Unknown";
   }
}

unsigned int ppmodify_build_plmn_subchunk ( t_treenode *node )
{
	t_workspace *ws = NULL;
	plmn_mnc mnc;
	plmn_hdr hdr;
	t_treenode *infonode = NULL;
	t_treenode *cntrnode = NULL;
	t_treenode *datanode = NULL;
	t_treenode *worknode = NULL;

	unsigned int length = 0;
	unsigned int pos = 0;
	char *buffer = NULL;

	memset ( &hdr, 0, sizeof ( plmn_hdr ) );

	if ( !node )
		PLUGIN_ERROR ( "(!node) failed", E_FAIL );

	infonode = treenode_get_byname ( treenode_children ( node ), "INFORMATION" );
	if ( !infonode )
		PLUGIN_ERROR ( "failed retrieving INFORMATION", E_FAIL );

	worknode = treenode_get_byname ( infonode, "ID" );
	if ( !worknode )
		PLUGIN_ERROR ( "failed retrieving INFORMATION/ID content", E_FAIL );
	hdr.cntr = treenode_get_content_data_hexval ( worknode );

	if ( hdr.cntr != 0x434E5452 )
		return ppmodify_build_ppm_subchunk ( node );

	worknode = treenode_get_byname ( infonode, "NAME" );
	if ( !worknode )
		PLUGIN_ERROR ( "failed retrieving INFORMATION/NAME content", E_FAIL );
	strcpy ( hdr.country, treenode_get_content_data ( worknode ) );

	worknode = treenode_get_byname ( infonode, "MCC" );
	if ( !worknode )
		PLUGIN_ERROR ( "failed retrieving INFORMATION/MCC content", E_FAIL );
	hdr.mcc = treenode_get_content_data_integer ( worknode );

	worknode = treenode_get_byname ( infonode, "VERSION" );
	if ( !worknode )
		PLUGIN_ERROR ( "failed retrieving INFORMATION/MCC content", E_FAIL );
	mnc_version = treenode_get_content_data_hexval ( worknode );

	worknode = treenode_get_byname ( infonode, "FLAGS" );
	if ( !worknode )
		PLUGIN_ERROR ( "failed retrieving INFORMATION/FLAGS content", E_FAIL );
	hdr.flags = treenode_get_content_data_hexval ( worknode );

   // get chunk data
   cntrnode = treenode_get_byname ( treenode_children ( node ), "NETWORKS" );
   if ( cntrnode )
   {
      length = 0x10;
      buffer = malloc ( length );
      pos = 0x10;
      
		datanode = treenode_get_byname ( treenode_children ( cntrnode ), "NETWORK" );
		while ( datanode )
		{
			hdr.count++;
         memset ( &mnc, 0, sizeof ( plmn_mnc ) );

         worknode = treenode_get_byname ( datanode, "EXTEND" );
         if ( worknode )
            mnc.extend = treenode_get_content_data_hexval ( worknode );

         worknode = treenode_get_byname ( datanode, "NAME" );
         if ( worknode )
            strcpy ( mnc.name, treenode_get_content_data ( worknode ) );

         mnc.namelen = strlen ( mnc.name );

         worknode = treenode_get_byname ( datanode, "MNC" );
         if ( worknode )
            mnc.mnc = treenode_get_content_data_integer ( worknode );

         if ( !mnc_version ) // dct3
         {
            mnc.length = 4 + mnc.namelen;
            while ( mnc.length & 3 )
               mnc.length++;

            length += mnc.length;
            buffer = realloc ( buffer, length );
            memset ( buffer + pos, 0, mnc.length );

            buffer[pos]     = mnc.length;
            buffer[pos + 1] = int2bcd ( mnc.mnc, 1 );
            buffer[pos + 2] = mnc.extend;
            buffer[pos + 3] = mnc.namelen;
            memcpy ( buffer + pos + 4, mnc.name, mnc.namelen );
            pos = length;
         }
         else
         {
            mnc.length = 5 + mnc.namelen;
            while ( mnc.length & 3 )
               mnc.length++;

            length += mnc.length;
            buffer = realloc ( buffer, length );
            memset ( buffer + pos, 0, mnc.length );

            buffer[pos]     = mnc.length;
            buffer[pos + 1] = mnc.extend;
            buffer[pos + 2] = mnc.mnc;
            buffer[pos + 4] = mnc.namelen;
            memcpy ( buffer + pos + 5, mnc.name, mnc.namelen );
            pos = length;
         }

			datanode = treenode_get_byname ( treenode_next ( datanode ), "NETWORK" );
		}
   }

   if ( hdr.mcc / 10 )
      buffer[12] = int2bcd ( hdr.mcc / 10, 2 );
   else
      buffer[12] = 0;
   buffer[13] = int2bcd ( hdr.mcc % 10, 2 );
   buffer[14] = hdr.count;
   buffer[15] = hdr.flags;

   ws = workspace_create_file_from_buffer ( buffer, length );
   if ( !ws )
      PLUGIN_ERROR ( "failed creating temporary workspace", E_FAIL );

   ws->flags &= ~FLAGS_ENDIANESS;
   if ( endianess_be )
      ws->flags |= FLAGS_ENDIANESS_BE;
   else
      ws->flags |= FLAGS_ENDIANESS_LE;

   v_set_w ( ws, 0, 0x434E5452 );
   v_set_w ( ws, 4, length );
   v_memcpy_put ( ws, 8, hdr.country, 4 );

	// replace with constructed data
	treenode_set_name ( node, "SUBCHUNK-BINARY" );
	treenode_set_content_data_binary ( node, buffer, length );

	workspace_release ( ws );

   return E_OK;
}

unsigned int ppmodify_plmn_subchunk_dump_plmn ( t_treenode *node, t_workspace *ws, unsigned int start, unsigned int length, unsigned int mnc_count )
{
	plmn_mnc mnc;
	t_treenode *headernode = NULL;
	t_treenode *worknode = NULL;
	unsigned int pos = start;

	if ( !length || !node || !ws || !mnc_count || !v_get_b ( ws, start ) )
		return E_FAIL;

	if ( v_get_b ( ws, start + 3 ) )
		mnc_version = 0;
	else
		mnc_version = 1;

	while ( mnc_count )
	{
		memset ( &mnc, 0, sizeof ( plmn_mnc ) );
		mnc.length = v_get_b ( ws, pos );

		if ( mnc_version )
		{
			mnc.extend = v_get_b ( ws, pos + 1 );
			mnc.mnc = v_get_b ( ws, pos + 2 );
			mnc.namelen = v_get_b ( ws, pos + 4 );
			v_memcpy_get ( ws, mnc.name, pos + 5, mnc.namelen );
		}
		else
		{
			mnc.mnc = bcd2int ( ws, pos + 1, 1 );
			mnc.extend = v_get_b ( ws, pos + 2 );
			mnc.namelen = v_get_b ( ws, pos + 3 );
			v_memcpy_get ( ws, mnc.name, pos + 4, mnc.namelen );
		}

		headernode = treenode_addchild ( node );
		treenode_set_name ( headernode, "NETWORK" );

			worknode = treenode_addchild ( headernode );
			treenode_set_name ( worknode, "NAME" );
			treenode_set_content_type ( worknode, TREENODE_TYPE_ASCII );
			treenode_set_content_data ( worknode, mnc.name );

			worknode = treenode_addchild ( headernode );
			treenode_set_name ( worknode, "MNC" );
			treenode_set_content_type ( worknode, TREENODE_TYPE_INTEGER );
			treenode_set_content_data_integer ( worknode, mnc.mnc );

			worknode = treenode_addchild ( headernode );
			treenode_set_name ( worknode, "EXTEND" );
			treenode_set_content_type ( worknode, TREENODE_TYPE_HEXVAL );
			treenode_set_content_data_hexval ( worknode, mnc.extend );

		pos += mnc.length;
		mnc_count--;
	}

	return E_OK;
}

unsigned int ppmodify_plmn_subchunk_dump ( t_workspace *ws, unsigned int start, unsigned int length, t_treenode *node )
{
	plmn_hdr header;
	t_treenode *headernode = NULL;
	t_treenode *worknode = NULL;
	char *buff = NULL;
	unsigned hdr_size=0;

	if ( !ws || !node || !v_valid ( ws, start ) )
		PLUGIN_ERROR ( "( !ws || !node || !v_valid ( ws, start ) ) failed", E_FAIL );

	header.cntr = v_get_w ( ws, start );
	header.length = v_get_w ( ws, start + 0x04 );
	v_memcpy_get ( ws, header.country, start + 0x08, 4 );
	header.mcc = bcd2int ( ws, start + 0x0C, 2 );
	header.count = v_get_b ( ws, start + 0x0E );
	header.flags = v_get_b ( ws, start + 0x0F );

	if( header.cntr != 0x434E5452 )
      return ppmodify_ppm_subchunk_dump ( ws, start, length, node );

   if ( v_get_b ( ws, start + 0x13 ) )
      mnc_version = 0;
   else
      mnc_version = 1;

	if ( header.length < 0x10 )
		header.length = 0x10;

	if ( header.length > length )
		PLUGIN_ERROR ( "header.length > length", E_FAIL );

	if ( header.length > v_get_size ( ws ) )
		PLUGIN_ERROR ( "header.length > filesize", E_FAIL );

	HEAP_CHECK;
	header.country[4] = '\000';
	if ( dump_chunks )
	{
		printf ( "    Position: 0x%08X\n", start );
		printf ( "    SubChunk: '%s'\n", header.country );
		printf ( "    Length  : 0x%08X\n", header.length );
		printf ( "    MCC     : %d\n", header.mcc );
		printf ( "    Flags   : 0x%02X\n", header.flags );
		printf ( "    Version : 0x%02X\n", mnc_version );
		printf ( "\n" );
	}
	treenode_set_name ( node, "SUBCHUNK" );

	headernode = treenode_addchild ( node );
	treenode_set_name ( headernode, "INFORMATION" );

	worknode = treenode_addchild ( headernode );
	treenode_set_name ( worknode, "ID" );
	treenode_set_content_type ( worknode, TREENODE_TYPE_HEXVAL );
	treenode_set_content_data_hexval ( worknode, header.cntr );

	worknode = treenode_addchild ( headernode );
	treenode_set_name ( worknode, "NAME" );
	treenode_set_content_type ( worknode, TREENODE_TYPE_ASCII );
	treenode_set_content_data ( worknode, header.country );

	worknode = treenode_addchild ( headernode );
	treenode_set_name ( worknode, "LENGTH" );
	treenode_set_content_type ( worknode, TREENODE_TYPE_HEXVAL );
	treenode_set_content_data_hexval ( worknode, header.length );

	worknode = treenode_addchild ( headernode );
	treenode_set_name ( worknode, "COUNTRY" );
	treenode_set_content_type ( worknode, TREENODE_TYPE_ASCII );
	treenode_set_content_data ( worknode, mcc2name ( header.mcc ) );

	worknode = treenode_addchild ( headernode );
	treenode_set_name ( worknode, "MCC" );
	treenode_set_content_type ( worknode, TREENODE_TYPE_INTEGER );
	treenode_set_content_data_integer ( worknode, header.mcc );

	worknode = treenode_addchild ( headernode );
	treenode_set_name ( worknode, "FLAGS" );
	treenode_set_content_type ( worknode, TREENODE_TYPE_HEXVAL );
	treenode_set_content_data_hexval ( worknode, header.flags );

	worknode = treenode_addchild ( headernode );
	treenode_set_name ( worknode, "VERSION" );
	treenode_set_content_type ( worknode, TREENODE_TYPE_HEXVAL );
	treenode_set_content_data_hexval ( worknode, mnc_version );

   headernode = treenode_addchild ( node );
   treenode_set_name ( headernode, "NETWORKS" );
   ppmodify_plmn_subchunk_dump_plmn ( headernode, ws, start + 0x10, header.length - 0x10, header.count );

	return E_OK;
}

#endif /* __TRIX_CHUNK_PLMN_C__ */
