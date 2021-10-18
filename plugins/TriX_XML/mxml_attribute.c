/*
   Mini XML lib

   Document routines

   Author: Giancarlo Niccolai <gian@niccolai.ws>

   $Id: mxml_attribute.c,v 1.3 2007/03/31 15:13:00 geggo Exp $
*/

#include "mxml.h"

// just a shortcut
#define MALFORMED_ERROR( elem, val ) \
   elem->status = MXML_STATUS_MALFORMED;\
   elem->error = val;

MXML_ATTRIBUTE *mxml_attribute_new()
{
   MXML_ATTRIBUTE *attribute = (MXML_ATTRIBUTE *)
         MXML_ALLOCATOR( sizeof( MXML_ATTRIBUTE ) );

   if ( attribute != NULL )
      mxml_attribute_setup( attribute );

   return attribute;
}

MXML_STATUS mxml_attribute_setup( MXML_ATTRIBUTE *attribute )
{
   attribute->name = NULL;
   attribute->value = NULL;
   mxml_attribute_unlink( attribute );

   return MXML_STATUS_OK;
}

/* Clones an attribute list */
MXML_ATTRIBUTE *mxml_attribute_clone( MXML_ATTRIBUTE *attrib )
{
   MXML_ATTRIBUTE *clone_head, *clone_tail;

   if( attrib == NULL )
      return NULL;

   clone_head = clone_tail = mxml_attribute_new();

   while ( attrib != NULL ) {
      if ( attrib->name != NULL ) {
         int nlen = strlen( attrib->name ) + 1;
         clone_tail->name = ( char *) MXML_ALLOCATOR( nlen );
         memcpy( clone_tail->name, attrib->name, nlen );
      }

      if ( attrib->value != NULL ) {
         int nlen = strlen( attrib->value ) + 1;
         clone_tail->value = ( char *) MXML_ALLOCATOR( nlen );
         memcpy( clone_tail->value, attrib->value, nlen );
      }


      attrib = attrib->next;

      if ( attrib != NULL ) {
         clone_tail->next = mxml_attribute_new();
         clone_tail = clone_tail->next;
      }

   }

   return clone_head;
}

void mxml_attribute_destroy( MXML_ATTRIBUTE *attrib )
{
   if ( attrib->name != NULL )
      MXML_DELETOR( attrib->name );

   if ( attrib->value != NULL )
      MXML_DELETOR( attrib->value );

   if ( attrib->next != NULL )
      mxml_attribute_destroy( attrib->next );

   MXML_DELETOR( attrib );
}


void mxml_attribute_unlink( MXML_ATTRIBUTE *attrib )
{
   attrib->next = NULL;
}


MXML_ATTRIBUTE *mxml_attribute_read( MXML_REFIL *ref, MXML_DOCUMENT *doc, int style )
{
   MXML_ATTRIBUTE *ret;
   int chr, quotechr;
   char buf_name[ MXML_MAX_NAME_LEN + 1];
   char buf_attrib[MXML_MAX_ATTRIB_LEN *2 + 1];
   int iPosn = 0, iPosa = 0;
   int iStatus = 0;
   int iPosAmper;

   while ( iStatus < 6 && iPosn <= MXML_MAX_NAME_LEN && iPosa <= MXML_MAX_ATTRIB_LEN ) {
      chr = mxml_refil_getc( ref );
      if ( chr == MXML_EOF ) break;

      switch ( iStatus ) {
         // begin
         case 0:
            switch ( chr ) {
               case MXML_LINE_TERMINATOR: doc->iLine++; break;
               // We repeat line terminator here for portability
               case MXML_SOFT_LINE_TERMINATOR: break;
               case ' ': case '\t': break;
               // no attributes found
               case '>': case '/': return NULL;
               default:
                  if ( isalpha( chr ) ) {
                     buf_name[ iPosn++ ] = chr;
                     iStatus = 1;
                  }
                  else {
                     MALFORMED_ERROR( doc,  MXML_ERROR_INVATT );
                     return NULL;
                  }
            }
         break;

         // scanning for a name
         case 1:
            if ( isalnum( chr ) || chr == '_' || chr == '-' || chr == ':' ) {
               buf_name[ iPosn++ ] = chr;
            }
            else if( chr == MXML_LINE_TERMINATOR ) {
               doc->iLine ++ ;
               iStatus = 2; // waiting for a '='
            }
            // We repeat line terminator here for portability
            else if ( chr == ' ' || chr == '\t' || chr == '\n' || chr == '\r' ) {
               iStatus = 2;
            }
            else if ( chr == '=' ) {
               iStatus = 3;
            }
            else {
               MALFORMED_ERROR( doc, MXML_ERROR_MALFATT );
               return NULL;
            }
         break;

         // waiting for '='
         case 2:
            if ( chr == '=' ) {
               iStatus = 3;
            }
            else if( chr == MXML_LINE_TERMINATOR ) {
               doc->iLine ++ ;
            }
            // We repeat line terminator here for portability
            else if ( chr == ' ' || chr == '\t' || chr == '\n' || chr == '\r' ) {
            }
            else {
               MALFORMED_ERROR( doc, MXML_ERROR_MALFATT );
               return NULL;
            }
         break;

         // waiting for ' or "
         case 3:
            if ( chr == '\'' || chr == '"' ) {
               iStatus = 4;
               quotechr = chr;
            }
            else if( chr == MXML_LINE_TERMINATOR ) {
               doc->iLine ++ ;
            }
            // We repeat line terminator here for portability
            else if ( chr == ' ' || chr == '\t' || chr == '\n' || chr == '\r' ) {
            }
            else {
               MALFORMED_ERROR( doc, MXML_ERROR_MALFATT );
               return NULL;
            }
         break;

         // scanning the attribute content ( until next quotechr )
         case 4:
            if ( chr == '&' && !( style & MXML_STYLE_NOESCAPE) ) {
               iStatus = 5;
               iPosAmper = iPosa;
               buf_attrib[ iPosa++ ] = chr; //we'll need it
            }
            else if( chr == MXML_LINE_TERMINATOR ) {
               doc->iLine ++ ;
               buf_attrib[ iPosa++ ] = chr;
            }
            else if ( chr == quotechr ) {
               iStatus = 6;
            }
            else {
               buf_attrib[ iPosa++ ] = chr;
            }
         break;

         case 5:
            if ( chr == quotechr ) {
               iStatus = 6;
            }
            else if ( chr == ';' ) {
               int iAmpLen = iPosa - iPosAmper - 2;
               char *bp = buf_attrib + iPosAmper + 1;

               if ( iAmpLen <= 0 ) {
                  //error! - we have "&;"
                  doc->status = MXML_STATUS_MALFORMED;
                  doc->error = MXML_ERROR_WRONGENTITY;
                  return NULL;
               }

               iStatus = 4;

               // we see if we have a predef entity (also known as escape)
               if ( strncmp( bp, "amp", iAmpLen ) == 0 ) chr = '&';
               else if ( strncmp( bp, "lt", iAmpLen ) == 0 ) chr = '<';
               else if ( strncmp( bp, "gt", iAmpLen ) == 0 ) chr = '>';
               else if ( strncmp( bp, "quot", iAmpLen ) == 0 ) chr = '"';
               else if ( strncmp( bp, "apos", iAmpLen ) == 0 ) chr = '\'';
               iPosa = iPosAmper;
               buf_attrib[ iPosa++ ] = chr;
            }
            else if ( ! isalpha( chr ) ) {
               //error - we have something like &amp &amp
               doc->status = MXML_STATUS_MALFORMED;
               doc->error = MXML_ERROR_UNCLOSEDENTITY;
               return NULL;
            }
            else {
               buf_attrib[ iPosa++ ] = chr;
            }
         break;
      }
   }

   if ( doc->status != MXML_STATUS_OK )
      return NULL;

   if ( iStatus < 6 ) {
      doc->status = MXML_STATUS_MALFORMED;
      if ( iPosn > MXML_MAX_NAME_LEN )
         doc->error = MXML_ERROR_ATTRIBTOOLONG;
      else if ( iPosa > MXML_MAX_ATTRIB_LEN )
         doc->error = MXML_ERROR_VALATTOOLONG;
      else
         doc->error = MXML_ERROR_MALFATT;

      return NULL;
   }

   // time to create the attribute
   ret = mxml_attribute_new();
   ret->name = (char *) MXML_ALLOCATOR( iPosn + 1);
   memcpy( ret->name, buf_name, iPosn );
   ret->name[ iPosn ] = 0;

   ret->value = (char *) MXML_ALLOCATOR( iPosa + 1 );
   memcpy( ret->value, buf_attrib, iPosa );
   ret->value[ iPosa ] = 0;

   return ret;
}

MXML_STATUS mxml_attribute_write( MXML_OUTPUT *out, MXML_ATTRIBUTE *attr, int style )
{

   mxml_output_string( out, attr->name );
   mxml_output_char( out, '=' );
   mxml_output_char( out, '"' );

   if ( style & MXML_STYLE_NOESCAPE )
      mxml_output_string( out, attr->value );
   else
      mxml_output_string_escape( out, attr->value );

   mxml_output_char( out, '"' );

   return out->status;
}
