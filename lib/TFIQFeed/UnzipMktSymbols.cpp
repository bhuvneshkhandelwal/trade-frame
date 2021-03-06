/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

//#include "StdAfx.h"

//#include <boost/scope_exit.hpp>

// http://blog.raymond.burkholder.net/index.php?/archives/598-Build-zlib-v1.2.8-with-Visual-Studio-2012.html

//#include <zlib/contrib/minizip/ioapi.h>
//#include <zlib/contrib/minizip/unzip.h>
#include <zlib/unzip.h>
#include <zlib/ioapi_mem.h>

#include <stdexcept>

//#include "CurlGetMktSymbols.h"
#include "UnzipMktSymbols.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

UnZipMktSymbolsFile::pUnZippedFile_t UnZipMktSymbolsFile::UnZip( char* pchSource, size_t nSourceSize ) {

//  CurlGetMktSymbols cgms;

  static const char* sZipFile( "**inmem**" );
  static const char* sSourceName( "mktsymbols_v2.txt" );

  unzFile uf=NULL;
  int err=UNZ_OK;
  unz_file_info64 file_info;
  char filename_inzip[256];
  ourmemory_t om;
  om.base = 0;
  om.cur_offset = om.limit = om.size = 0;

//  om.base = cgms.Buffer();
//  om.size = cgms.Size();
  om.base = pchSource;
  om.size = nSourceSize;

//  char* pchUnzippedFileContent;
//  pchUnzippedFileContent = 0;

//  BOOST_SCOPE_EXIT( &pchUnzippedFileContent ) {
//    if ( 0 != pchUnzippedFileContent ) {
//      delete[] pchUnzippedFileContent;
//      pchUnzippedFileContent = 0;
//    }
//  } BOOST_SCOPE_EXIT_END

  zlib_filefunc64_def ffunc;

  fill_memory_filefunc64( &ffunc, &om );

  uf = unzOpen2_64(sZipFile, &ffunc);
  if ( 0 == uf ) 
    throw std::runtime_error( "open" );

  err = unzLocateFile( uf, sSourceName, 0 );
  if ( UNZ_OK != err ) 
    throw  std::runtime_error( "locate" );

  err = unzGetCurrentFileInfo64(uf,&file_info,filename_inzip,sizeof(filename_inzip),NULL,0,NULL,0);
  if ( UNZ_OK != err ) 
    throw  std::runtime_error( "get info" );

  err = unzOpenCurrentFile( uf );
  if ( UNZ_OK != err ) 
    throw  std::runtime_error( "open current" );

  m_nUnZippedFileSize = file_info.uncompressed_size;
  assert( 0 != m_nUnZippedFileSize );
  pUnZippedFile_t pUnZippedFileContent;
  pUnZippedFileContent.reset( new char[ file_info.uncompressed_size ] ); 
//  pchUnzippedFileContent = new char[ file_info.uncompressed_size ];
  if ( 0 == pUnZippedFileContent.get() ) {
    throw  std::runtime_error( "UnzippedFileContent" );
  }

  int cnt = unzReadCurrentFile(uf,pUnZippedFileContent.get(),file_info.uncompressed_size);

  if ( file_info.uncompressed_size != cnt ) 
    throw  std::runtime_error( "read" );

  err = unzCloseCurrentFile( uf );
  if ( UNZ_OK != err ) 
    throw  std::runtime_error( "close current" );

  err = unzClose( uf );
  if ( UNZ_OK != err ) 
    throw  std::runtime_error( "close" );

  return pUnZippedFileContent;
}

} // namespace iqfeed
} // namespace tf
} // namespace ou
