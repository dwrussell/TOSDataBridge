/* 
Copyright (C) 2014 Jonathon Ogden   < jeog.dev@gmail.com >

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses.
*/

#include "shell.hpp"

namespace{
  
void GetStreamSnapshotDoubles(CommandCtx *ctx);
void GetStreamSnapshotFloats(CommandCtx *ctx); 
void GetStreamSnapshotLongLongs(CommandCtx *ctx);
void GetStreamSnapshotLongs(CommandCtx *ctx);
void GetStreamSnapshotStrings(CommandCtx *ctx);
void GetStreamSnapshotGenerics(CommandCtx *ctx);
void GetStreamSnapshotDoublesFromMarker(CommandCtx *ctx);
void GetStreamSnapshotFloatsFromMarker(CommandCtx *ctx); 
void GetStreamSnapshotLongLongsFromMarker(CommandCtx *ctx);
void GetStreamSnapshotLongsFromMarker(CommandCtx *ctx);
void GetStreamSnapshotStringsFromMarker(CommandCtx *ctx);


commands_map_ty
build_commands_map()
{
    commands_map_ty m;

    m.insert( build_commands_map_elem("GetStreamSnapshotDoubles",GetStreamSnapshotDoubles) );
    m.insert( build_commands_map_elem("GetStreamSnapshotFloats",GetStreamSnapshotFloats) );
    m.insert( build_commands_map_elem("GetStreamSnapshotLongLongs",GetStreamSnapshotLongLongs) );
    m.insert( build_commands_map_elem("GetStreamSnapshotLongs",GetStreamSnapshotLongs) );
    m.insert( build_commands_map_elem("GetStreamSnapshotStrings",GetStreamSnapshotStrings) );
    m.insert( build_commands_map_elem("GetStreamSnapshotGenerics",GetStreamSnapshotGenerics) );
    m.insert( build_commands_map_elem("GetStreamSnapshotDoublesFromMarker",GetStreamSnapshotDoublesFromMarker) );
    m.insert( build_commands_map_elem("GetStreamSnapshotFloatsFromMarker",GetStreamSnapshotFloatsFromMarker) );
    m.insert( build_commands_map_elem("GetStreamSnapshotLongLongsFromMarker",GetStreamSnapshotLongLongsFromMarker) );
    m.insert( build_commands_map_elem("GetStreamSnapshotLongsFromMarker",GetStreamSnapshotLongsFromMarker) );
    m.insert( build_commands_map_elem("GetStreamSnapshotStringsFromMarker",GetStreamSnapshotStringsFromMarker) );
    
    return m;
}

};


commands_map_ty commands_stream = build_commands_map();


namespace{

template<typename T>
void 
_get_stream_snapshot( int(*func)(LPCSTR, LPCSTR, LPCSTR, T*, 
                                 size_type, pDateTimeStamp, long, long),
                      CommandCtx *ctx );

template<typename T>
void _get_stream_snapshot(CommandCtx *ctx);

template<typename T>
void 
_get_stream_snapshot_from_marker( int(*func)(LPCSTR, LPCSTR, LPCSTR, T*,
                                             size_type, pDateTimeStamp, long, long*),
                                  CommandCtx *ctx );


void
GetStreamSnapshotDoubles(CommandCtx *ctx)
{
    prompt_for_cpp(ctx) 
        ? _get_stream_snapshot<double>(ctx) 
        : _get_stream_snapshot<double>(TOSDB_GetStreamSnapshotDoubles, ctx);
}

void
GetStreamSnapshotFloats(CommandCtx *ctx)
{
    prompt_for_cpp(ctx) 
        ? _get_stream_snapshot<float>(ctx) 
        : _get_stream_snapshot<float>(TOSDB_GetStreamSnapshotFloats, ctx);
}

void
GetStreamSnapshotLongLongs(CommandCtx *ctx)
{
    prompt_for_cpp(ctx) 
        ? _get_stream_snapshot<long long>(ctx) 
        : _get_stream_snapshot<long long>(TOSDB_GetStreamSnapshotLongLongs, ctx);
}

void
GetStreamSnapshotLongs(CommandCtx *ctx)
{
    prompt_for_cpp(ctx) 
        ? _get_stream_snapshot<long>(ctx) 
        : _get_stream_snapshot<long>(TOSDB_GetStreamSnapshotLongs, ctx);
}

void
GetStreamSnapshotStrings(CommandCtx *ctx)
{
    if(prompt_for_cpp(ctx)){
        _get_stream_snapshot<std::string>(ctx);
    }else{        
        size_type display_len;           
        int ret;
        std::string block;
        std::string item;
        std::string topic;
        std::string len_s;
        std::string beg_s;
        std::string end_s;
        bool get_dts;
                        
        char **dat = nullptr;
        pDateTimeStamp dts = nullptr;

        prompt_for_block_item_topic(&block, &item, &topic, ctx);
        prompt_for("length of array", &len_s, ctx);        
        prompt_for("beginning datastream index", &beg_s, ctx);        
        prompt_for("ending datastream index", &end_s, ctx);

        size_type len = 0;
        long beg = 0;
        long end = 0;
        try{
            len = std::stoul(len_s);
            beg = std::stol(beg_s);
            end = std::stol(end_s);
        }catch(...){
            std::cerr<< std::endl << "INVALID INPUT" << std::endl << std::endl;
            return;
        }
        
        get_dts = prompt_for_datetime(block, ctx);

        try{
            dat = NewStrings(len, TOSDB_STR_DATA_SZ - 1);

            if(get_dts)
                dts = new DateTimeStamp[len];

            ret = TOSDB_GetStreamSnapshotStrings(block.c_str(), item.c_str(), topic.c_str(),
                                                  dat, len, TOSDB_STR_DATA_SZ, dts, end, beg);
           
            display_len = min_stream_len(block,beg,end,len);

            check_display_ret(ret, dat, display_len, dts);

            DeleteStrings(dat, len);
            if(dts)
                delete dts;
        }catch(...){
            DeleteStrings(dat, len);
            if(dts)
                delete dts;        
            throw;
        }            
    }
}


void
GetStreamSnapshotGenerics(CommandCtx *ctx)
{
    _get_stream_snapshot<generic_type>(ctx);
}

void
GetStreamSnapshotDoublesFromMarker(CommandCtx *ctx)
{
    _get_stream_snapshot_from_marker<double>(TOSDB_GetStreamSnapshotDoublesFromMarker, ctx);
}


void
GetStreamSnapshotFloatsFromMarker(CommandCtx *ctx)
{
    _get_stream_snapshot_from_marker<float>(TOSDB_GetStreamSnapshotFloatsFromMarker, ctx);
}

void
GetStreamSnapshotLongLongsFromMarker(CommandCtx *ctx)
{
    _get_stream_snapshot_from_marker<long long>(TOSDB_GetStreamSnapshotLongLongsFromMarker, ctx);
}


void
GetStreamSnapshotLongsFromMarker(CommandCtx *ctx)
{
    _get_stream_snapshot_from_marker<long>(TOSDB_GetStreamSnapshotLongsFromMarker, ctx);
}


void
GetStreamSnapshotStringsFromMarker(CommandCtx *ctx)
{    
    long get_size;
    int ret;
    std::string block;
    std::string item;
    std::string topic;
    std::string len_s;
    std::string beg_s;
    bool get_dts;

    char **dat = nullptr;
    pDateTimeStamp dts= nullptr;

    prompt_for_block_item_topic(&block, &item, &topic, ctx);

    prompt_for("length of array", &len_s, ctx);        
    prompt_for("beginning datastream index", &beg_s, ctx);        
      
    size_type len = 0;
    long beg = 0;      
    try{
        len = std::stoul(len_s);
        beg = std::stol(beg_s);      
    }catch(...){
        std::cerr<< std::endl << "INVALID INPUT" << std::endl << std::endl;
        return;
    }

    get_dts = prompt_for_datetime(block, ctx);

    try{         
        dat = NewStrings(len, TOSDB_STR_DATA_SZ - 1);

        if(get_dts)
            dts = new DateTimeStamp[len];

        ret = TOSDB_GetStreamSnapshotStringsFromMarker(block.c_str(), item.c_str(), topic.c_str(), 
                                                        dat, len, TOSDB_STR_DATA_SZ, 
                                                        dts, beg, &get_size);

        check_display_ret(ret, dat, abs(get_size), dts);

        DeleteStrings(dat, len);
        if(dts)
            delete dts;
    }catch(...){
        DeleteStrings(dat, len);
        if(dts)
            delete dts;
    }       
}



template<typename T>
void 
_get_stream_snapshot(CommandCtx *ctx)
{  
    std::string block;
    std::string item;
    std::string topic;
    std::string beg_s;
    std::string end_s;
    bool get_dts;

    prompt_for_block_item_topic(&block, &item, &topic, ctx);         
    prompt_for("beginning datastream index", &beg_s, ctx);        
    prompt_for("ending datastream index", &end_s, ctx);
      
    long beg = 0;
    long end = 0;
    try{
        beg = std::stol(beg_s);
        end = std::stol(end_s);
    }catch(...){
        std::cerr<< std::endl << "INVALID INPUT" << std::endl << std::endl;
        return;
    }

    get_dts = prompt_for_datetime(block, ctx);

    if(get_dts)
        std::cout<< std::endl 
                 << TOSDB_GetStreamSnapshot<T,true>(block.c_str(), item.c_str(), 
                                                    TOS_Topics::MAP()[topic], end, beg);               
    else
        std::cout<< std::endl 
                 << TOSDB_GetStreamSnapshot<T,false>(block.c_str(), item.c_str(), 
                                                     TOS_Topics::MAP()[topic], end, beg)
                 << std::endl;

    std::cout<< std::endl;
}


template<typename T>
void 
_get_stream_snapshot( int(*func)(LPCSTR, LPCSTR, LPCSTR, T*,
                                 size_type, pDateTimeStamp, long, long),
                      CommandCtx *ctx )
{  
    size_type display_len;   
    int ret;
    std::string block;
    std::string item;
    std::string topic;
    std::string len_s;
    std::string beg_s;
    std::string end_s;
    bool get_dts;

    pDateTimeStamp dts = nullptr;
    T *dat = nullptr;
   
    prompt_for_block_item_topic(&block, &item, &topic, ctx);
    prompt_for("length of array", &len_s, ctx);        
    prompt_for("beginning datastream index", &beg_s, ctx);        
    prompt_for("ending datastream index", &end_s, ctx);

    size_type len = 0;
    long beg = 0;
    long end = 0;
    try{
        len = std::stoul(len_s);
        beg = std::stol(beg_s);
        end = std::stol(end_s);
    }catch(...){
        std::cerr<< std::endl << "INVALID INPUT" << std::endl << std::endl;
        return;
    }
    
    get_dts = prompt_for_datetime(block, ctx);

    try{
        dat = new T[len];

        if(get_dts)
            dts = new DateTimeStamp[len];

        ret = func(block.c_str(), item.c_str(), topic.c_str(), dat, len, dts, end, beg);

        display_len = min_stream_len(block,beg,end,len);

        check_display_ret(ret, dat, display_len, dts);
 
        if(dat)
            delete[] dat;
        if(dts)
            delete[] dts;
    }catch(...){
        if(dat)
            delete[] dat;
        if(dts)
            delete[] dts;
    }
}


template<typename T>
void 
_get_stream_snapshot_from_marker( int(*func)(LPCSTR, LPCSTR, LPCSTR, T*,
                                             size_type, pDateTimeStamp, long, long*),
                                  CommandCtx *ctx )
{   
    long get_size;
    int ret;
    std::string block;
    std::string item;
    std::string topic;
    std::string beg_s;
    std::string len_s;
    bool get_dts;

    pDateTimeStamp dts = nullptr;
    T *dat = nullptr;

    prompt_for_block_item_topic(&block, &item, &topic, ctx);
    prompt_for("length of array", &len_s, ctx);        
    prompt_for("beginning datastream index", &beg_s, ctx);         

    size_type len = 0;
    long beg = 0;  
    try{
        len = std::stoul(len_s);
        beg = std::stol(beg_s);          
    }catch(...){
        std::cerr<< std::endl << "INVALID INPUT" << std::endl << std::endl;
        return;
    }

    get_dts = prompt_for_datetime(block, ctx);

    try{
        dat = new T[len];

        if(get_dts)
            dts = new DateTimeStamp[len];

        ret = func(block.c_str(), item.c_str(), topic.c_str(), dat, len, dts, beg, &get_size);

        check_display_ret(ret, dat, abs(get_size), dts);

        if(dat)
            delete[] dat;
        if(dts)
            delete[] dts;
    }catch(...){
        if(dat)
            delete[] dat;
        if(dts)
            delete[] dts;
    }
}


};