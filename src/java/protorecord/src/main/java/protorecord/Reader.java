/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package protorecord;

import java.io.BufferedInputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;
import protorecord.ProtorecordIndex.IndexItem;
import protorecord.ProtorecordIndex.IndexSummary;
import protorecord.ProtorecordTypes.Version;

/**
 *
 * @author daniel
 */
public class Reader {
    // set to true if the writer was initialized succesfully
    private boolean initialized_;

    // the parsed library version from the record
    private int version_major_;
    private int version_minor_;
    private int version_patch_;

    // the parsed IndexSummary
    private ProtorecordIndex.IndexSummaryOrBuilder index_summary_;
    
    private long index_summary_total_items_;
    private int index_summary_index_item_size_;
    private long index_summary_start_time_utc_;
    private int index_summary_flags_;

    // the current parsed index item
    private long index_item_offset_;
    private int index_item_size_;

    // the opened index file
    private FileInputStream index_file_;

    // the opened data file
    private FileInputStream data_file_;

    // buffer used to deserialize data from files
//    private std::vector<char> buffer_;

    // the next item index the class will read from
    private long next_item_num_;

    // set to true if any internal failure occurs
    private boolean failbit_;

    // set to a human reasble string explaing previous method's failure
    private String fail_reason_;
    
    /**
     * Constructor
     *
     * @param[in] filepath
     * The absolute or relative filepath to the record.
     */
    public Reader(String filepath)
    {
        index_summary_ = IndexSummary.getDefaultInstance();
        index_file_ = null;
        data_file_ = null;
        next_item_num_ = 0;
        failbit_ = false;
        fail_reason_ = "";
        
        initialized_ = init_record(filepath);
    }
 
    /**
     * @return
     * True if there is at least one more item that can be read
     * from the record.
     */
    public boolean has_next()
    {
        fail_reason_ = "";
        return initialized_ &&
                ! failbit_ &&
                next_item_num_ < index_summary_.getTotalItems();
    }
 
    /**
     * Reads the next protobuf message from the record, but does not
     * increment to the next item.
     *
     * @param[out] pb
     * The google::protobuf message to read into
     *
     * @return
     * True if message was successfully read, false otherwise
     */
//    boolean get_next(com.google.protobuf.Any pb)
//    {
//    }
 
    /**
     * Reads the next protobuf message from the record, and increments
     * to the next item.
     *
     * @param[out] pb
     * The google::protobuf message to read into
     *
     * @return
     * True if message was successfully read, false otherwise
     */
//    template<class PROTOBUF_T>
//    bool
//    take_next(
//            PROTOBUF_T &pb);
 
    /**
     * Reads the next item's timestamp
     *
     * @return
     * The parsed item's timestamp
     */
    public long get_next_timestamp()
    {
        fail_reason_ = "";
        IndexItem next_item = get_index_item(next_item_num_);
        if (next_item != null && next_item.hasTimestamp())
        {
            return next_item.getTimestamp();
        }
        // FIXME raise exception?
        return 0;
    }
 
    /**
     * @return
     * The number of items that can be read from the record
     */
    public long size()
    {
        fail_reason_ = "";
        if (initialized_)
        {
            return index_summary_.getTotalItems();
        }
        return 0;
    }
 
    /**
     * @return
     * The record's bit mask of protorecord::Flags::* constants.
     */
    public int flags()
    {
        if (initialized_)
        {
            fail_reason_ = "";
            return index_summary_.getFlags();
        }
        else
        {
            fail_reason_ = "Reader not initialized";
            return 0;
        }
    }
 
    /**
     * @return
     * True if the record's HAS_ASSUMED_DATA flag is set, false otherwise.
     */
    public boolean has_assumed_data()
    {
        fail_reason_ = "";
        return is_flag_set(Constants.Flags.HAS_ASSUMED_DATA);
    }
 
    /**
     * @return
     * True if the record contains timestamped items, false otherwise.
     */
    public boolean has_timestamps()
    {
        fail_reason_ = "";
        return is_flag_set(Constants.Flags.HAS_TIMESTAMPS);
    }
 
    /**
     * @param[out] start_time_us
     * The records start time in microseconds
     *
     * @return
     * True on success, false otherwise. If false is returned, the value
     * of 'start_time_us' should not be assumed.
     */
    public long get_start_time()
    {
        fail_reason_ = "";
        if ( ! initialized_)
        {
            fail_reason_ = "Reader not initialized";
            // TODO raise an exception?
            return -1;
        }
        return index_summary_.getStartTimeUtc();
    }
 
    /**
     * @return
     * The protorecord library version that the record was made with.
     * This can be used to determine version incompatibilities. Returns
     * version 0.0.0 if Reader was not initialized properly.
     */
    public ProtorecordTypes.VersionOrBuilder get_version()
    {
        fail_reason_ = "";
        if ( ! initialized_)
        {
            fail_reason_ = "Reader not initialized";
        }
        return ProtorecordTypes.Version.newBuilder()
                .setMajor(version_major_)
                .setMinor(version_minor_)
                .setPatch(version_patch_)
                .build();
    }
 
    /**
     * @return
     * A string explaining the failure reason for a previously called
     * method in this class. An empty string is returned if the previous
     * method was successful. This method will also return an empty
     * string upon subsequent calls, in affect "popping" the failure
     * reason from the class.
     */
    public String reason()
    {
        return fail_reason_;
    }
 
    /**
     * Initializes the class for read. This method is called from
     * the constructor and is used to setup the internal member
     * variables.
     *
     * @param[in] filepath
     * The record path to attempt to initial the class with
     *
     * @return
     * True if successfully initialized, false otherwise
     */
    protected boolean init_record(String filepath)
    {
        fail_reason_ = "";
        boolean okay = true;

        // open the index file
        String INDEX_FILEPATH = filepath + "/index";
        if (okay)
        {
            try {
                index_file_ = new FileInputStream(INDEX_FILEPATH);
            } catch (FileNotFoundException ex) {
                fail_reason_ = "failed to open index file '" + INDEX_FILEPATH + "'";
                okay = false;
            }
        }

        // open the data file
        String DATA_FILEPATH = filepath + "/data";
        if (okay)
        {
            try {
                data_file_ = new FileInputStream(DATA_FILEPATH);
            } catch (FileNotFoundException ex) {
                fail_reason_ = "failed to open data file '" + DATA_FILEPATH + "'";
                okay = false;
            }
        }

        // set version to invalid default
        version_major_ = 0;
        version_minor_ = 0;
        version_patch_ = 0;

        // intialize index item
        index_item_offset_ = 0;
        index_item_size_ = 0;

        // initialize index summary
        index_summary_total_items_ = 0;
        index_summary_index_item_size_ = 12;// FIXME set for timestamped items
        index_summary_start_time_utc_ = 0;
        index_summary_flags_ = 0;

        try
        {
            // read library version from record
            if (okay)
            {
                byte[] buffer = null;
                index_file_.read(buffer,0,Constants.PROTORECORD_VERSION_SIZE);
                Version version = Version.parseFrom(buffer);
                version_major_ = version.getMajor();
                version_minor_ = version.getMinor();
                version_patch_ = version.getPatch();

                // check for compatibility
                if ( ! is_compatible(version))
                {
                    fail_reason_ = "file/library version incompatibility";
                    okay = false;
                }
            }

            // read IndexSummary from record
            if (okay)
            {
                byte[] buffer = null;
                index_file_.read(
                        buffer,
                        Constants.PROTORECORD_VERSION_SIZE,
                        Constants.PROTORECORD_INDEX_SUMMARY_SIZE);

                IndexSummary summary = IndexSummary.parseFrom(buffer);
                index_summary_total_items_ = summary.getTotalItems();
                index_summary_index_item_size_ = summary.getIndexItemSize();
                index_summary_start_time_utc_ = summary.getStartTimeUtc();
                index_summary_flags_ = summary.getFlags();
            }
        } catch (IOException ex) {
            fail_reason_ = "caught IOException ";
            fail_reason_ += ex.toString();
            okay = false;
        }

        return okay;
    }
 
    /**
     * Closes all opened file descriptors. This method is automatically
     * called by class's destructor.
     */
    public void close()
    {
        try {
            index_file_.close();
        } catch (IOException ex) {
        }
        try {
            data_file_.close();
        } catch (IOException ex) {
        }
    }
 
    /**
     * @return
     * True if record's version number is compatible with this version of
     * Reader class.
     */
    private boolean is_compatible(Version record_version)
    {
        return true;
    }
 
    /**
     * Parse the next index item from the index_file_ and places the
     * result in the classes index_item_ member
     *
     * @param[in] item_idx
     * The index item to read from the index_file
     *
     * @param[out] item_out
     * The parsed IndexItem
     *
     * @return
     * True if index_item was parsed successfully, false otherwise
     */
    private IndexItem get_index_item(long item_idx)
    {
        fail_reason_ = "";
        boolean okay = initialized_ && item_idx < size();
        IndexItem item_out = null;

        if (okay)
        {
            // compute position to IndexItem in file
            int pos = Constants.PROTORECORD_VERSION_SIZE + Constants.PROTORECORD_INDEX_SUMMARY_SIZE;
            pos += item_idx * index_summary_.getIndexItemSize();

            // seek to position and read
            byte[] buffer = null;
            try {
                index_file_.read(buffer,pos,index_summary_.getIndexItemSize());
                item_out = IndexItem.parseFrom(buffer);
            } catch (IOException ex) {
                fail_reason_ = "reached end of index file";
                okay = false;
            }
        }

        return item_out;
    }
 
    /**
     * @param[in] flag
     * The flag to check for
     *
     * @return
     * True if the record flags are valid and the flag is set
     */
    private boolean is_flag_set(int flag)
    {
        int flags = this.flags();
        boolean is_set = true;
        is_set = is_set && (flags & Constants.Flags.VALID) > 0;
        is_set = is_set && (flags & flag) > 0;
        return is_set;
    }
}
