package protorecord;


import java.io.File;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author daniel
 */
public class TestRecordings {
    public static File testRecordsRoot() {
        File testRecords = new File("../../../test/records");
        try {
            testRecords = testRecords.getCanonicalFile();
        } catch (IOException ex) {
            Logger.getLogger(TestRecordings.class.getName()).log(Level.SEVERE, null, ex);
        }
        return testRecords;
    }
    
    public static File basicHelloworld() {
        return new File(testRecordsRoot(),"basic_helloworld");
    }
}
