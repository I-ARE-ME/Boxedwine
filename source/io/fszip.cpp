#include "boxedwine.h"
#ifdef BOXEDWINE_ZLIB
#undef OF
#define STRICTUNZIP
extern "C"
{
    #include "../../lib/zlib/contrib/minizip/unzip.h"
}
#include "fsfilenode.h"
#include "fszip.h"
#include "fszipnode.h"
#include <time.h> 

void FsZip::setupZipRead(U64 zipOffset, U64 zipFileOffset) {
#ifdef BOXEDWINE_ZLIB
    char tmp[4096];

    if (zipOffset != lastZipOffset || zipFileOffset < lastZipFileOffset) {
        unzCloseCurrentFile(FsZip::zipfile);
        unzSetOffset64(FsZip::zipfile, zipOffset);
        lastZipFileOffset = 0;
        unzOpenCurrentFile(FsZip::zipfile);
        lastZipOffset = zipOffset;
    }
    if (zipFileOffset != lastZipFileOffset) {
        U32 todo = (U32)(zipFileOffset - lastZipFileOffset);
        while (todo) {
            todo-=unzReadCurrentFile(FsZip::zipfile, tmp, todo>4096?4096:todo);
        }
    }  
#endif
}

bool FsZip::init(const std::string& zipPath, const std::string& mount) {
#ifdef BOXEDWINE_ZLIB
    this->lastZipOffset = 0xFFFFFFFFFFFFFFFFl;
    if (zipPath.length()) {
        unz_global_info global_info;
        U32 i;
        fsZipInfo* zipInfo;

        zipfile = unzOpen(zipPath.c_str());
        if (!zipfile) {
            klog("Could not load zip file: %s", zipPath.c_str());
        }

        if (unzGetGlobalInfo( zipfile, &global_info ) != UNZ_OK) {
            klog("Could not read file global info from zip file: %s", zipPath.c_str());
            unzClose( zipfile );
            return false;
        }
        zipInfo = new fsZipInfo[global_info.number_entry];
        for (i = 0; i < global_info.number_entry; ++i) {
            unz_file_info file_info;
            struct tm tm={0};
            char tmp[MAX_FILEPATH_LEN];

            zipInfo[i].filename="/";
            if ( unzGetCurrentFileInfo(zipfile, &file_info, tmp, MAX_FILEPATH_LEN, NULL, 0, NULL, 0 ) != UNZ_OK ) {
                klog("Could not read file info from zip file: %s", zipPath.c_str());
                unzClose( zipfile );
                return false;
            }
            zipInfo[i].filename.append(tmp);
            zipInfo[i].offset = unzGetOffset64(zipfile);
            Fs::remoteNameToLocal(zipInfo[i].filename); // converts special characters like :
            if (stringHasEnding(zipInfo[i].filename, ".link")) {
                U32 read;

                zipInfo[i].filename.resize(zipInfo[i].filename.length()-5);
                zipInfo[i].isLink = true;
                unzOpenCurrentFile(zipfile);
                read = unzReadCurrentFile(zipfile, tmp, MAX_FILEPATH_LEN);
                tmp[read]=0;
                zipInfo[i].link = tmp;                
                unzCloseCurrentFile(zipfile);
            }                       
            
            if (stringHasEnding(zipInfo[i].filename, "/")) {
                zipInfo[i].filename.resize(zipInfo[i].filename.length()-1);
                zipInfo[i].isDirectory = true;
            } else {
                zipInfo[i].length = file_info.uncompressed_size;
            }               
            tm.tm_sec = file_info.tmu_date.tm_sec;
            tm.tm_min = file_info.tmu_date.tm_min;
            tm.tm_hour = file_info.tmu_date.tm_hour;
            tm.tm_mday = file_info.tmu_date.tm_mday;
            tm.tm_mon = file_info.tmu_date.tm_mon;
            tm.tm_year = file_info.tmu_date.tm_year;
            if (tm.tm_year>1900)
                tm.tm_year-=1900;

            zipInfo[i].lastModified = ((U64)mktime(&tm))*1000l;

            unzGoToNextFile(zipfile);
        }
        if (0) {
            Fs::makeLocalDirs(mount);
        }
        for (i = 0; i < global_info.number_entry; ++i) {
            std::string parentPath = Fs::getParentPath(zipInfo[i].filename);            
            BoxedPtr<FsNode> parent = Fs::getNodeFromLocalPath("", parentPath, true);
            std::string localFileName = zipInfo[i].filename;
            Fs::remoteNameToLocal(localFileName);      
            localFileName = Fs::getFileNameFromPath(localFileName);
            BoxedPtr<FsFileNode> node = (FsFileNode*)Fs::addFileNode(zipInfo[i].filename, zipInfo[i].link, Fs::getNativePathFromParentAndLocalFilename(parent, localFileName), zipInfo[i].isDirectory, parent).get();
            node->zipNode = new FsZipNode(node, zipInfo[i], this);
        }   
        delete[] zipInfo;
    }
#endif
    return true;
}

bool FsZip::readFileFromZip(const std::string& zipFile, const std::string& file, std::string& result) {
    unzFile z = unzOpen(zipFile.c_str());
    unz_global_info global_info;
    if (!z) {
        return false;
    }
    if (unzGetGlobalInfo( z, &global_info ) != UNZ_OK) {
        unzClose( z );
        return false;
    }
    for (U32 i = 0; i < global_info.number_entry; ++i) {
        unz_file_info file_info;
        char tmp[MAX_FILEPATH_LEN];

        if ( unzGetCurrentFileInfo(z, &file_info, tmp, MAX_FILEPATH_LEN, NULL, 0, NULL, 0 ) != UNZ_OK ) {
            unzClose( z );
            return false;
        }
        
        if (file == tmp) {
            U32 read;

            unzOpenCurrentFile(z);            
            read = unzReadCurrentFile(z, tmp, MAX_FILEPATH_LEN);
            tmp[read]=0;              
            unzCloseCurrentFile(z);
            unzClose(z);
            result = tmp;
            return true;
        }
        unzGoToNextFile(z);
    }
    unzClose(z);
    return false;
}

bool FsZip::extractFileFromZip(const std::string& zipFile, const std::string& file, const std::string& path) {
    unzFile z = unzOpen(zipFile.c_str());
    unz_global_info global_info;
    if (!z) {
        return false;
    }
    if (unzGetGlobalInfo( z, &global_info ) != UNZ_OK) {
        unzClose( z );
        return false;
    }
    for (U32 i = 0; i < global_info.number_entry; ++i) {
        unz_file_info file_info;
        char tmp[MAX_FILEPATH_LEN];

        if ( unzGetCurrentFileInfo(z, &file_info, tmp, MAX_FILEPATH_LEN, NULL, 0, NULL, 0 ) != UNZ_OK ) {
            unzClose( z );
            return false;
        }
        
        if (file == tmp) {
            unzOpenCurrentFile(z);            
            if (!Fs::doesNativePathExist(path)) {
                Fs::makeNativeDirs(path);
            }
            std::string outPath = path+Fs::nativePathSeperator+Fs::getFileNameFromPath(file);
            FILE* f = fopen(outPath.c_str(), "wb");
            if (f) {
                U32 totalRead = 0;
                U8 buffer[4096];

                while (totalRead<file_info.uncompressed_size) {
                    U32 read = unzReadCurrentFile(z, buffer, sizeof(buffer));
                    if (!read) {
                        break;
                    }
                    totalRead += read;
                    fwrite(buffer, read, 1, f);
                }
                fclose(f);
                unzCloseCurrentFile(z);
                unzClose(z);
                return totalRead==file_info.uncompressed_size;
            } else {
                unzCloseCurrentFile(z);
                unzClose(z);
                return false;
            }
        }
        unzGoToNextFile(z);
    }
    unzClose(z);
    return false;
}

#endif
