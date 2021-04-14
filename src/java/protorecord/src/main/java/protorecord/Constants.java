/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package protorecord;

/**
 *
 * @author daniel
 */
public class Constants {
    // size in bytes of the Version message
    public static int PROTORECORD_VERSION_SIZE = 24;

    // size in bytes of the IndexSummary message
    public static int PROTORECORD_INDEX_SUMMARY_SIZE = 28;

    // size in bytes of the IndexItem message
    public static int PROTORECORD_INDEX_ITEM_SIZE_TIMESTAMP = 20;

    // size in bytes of the IndexItem message with no timestamp
    public static int PROTORECORD_INDEX_ITEM_SIZE_NO_TIMESTAMP = 12;
    
    static class Flags {
        // set if the other bits in the words are valid
        public static int VALID = 0x1;

        // set if the record experienced an error at some point
        // during the recording.
        public static int RECORD_WRITE_ERROR = 0x2;

        // set if the record contains binary message data that was assumed
        // to be serialized externally to library. This flag is set the first
        // time Writer::write_assumed() method is called.
        public static int HAS_ASSUMED_DATA = 0x4;

        // indicating the record contains timestamped items
        public static int HAS_TIMESTAMPS = 0x8;
    }
}
