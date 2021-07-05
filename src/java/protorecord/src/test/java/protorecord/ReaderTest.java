/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package protorecord;

import java.io.File;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.AfterAll;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;
import protorecord.demo.DemoMessages;

/**
 *
 * @author daniel
 */
public class ReaderTest {
    
    File basicHelloworld = TestRecordings.basicHelloworld();
    
    public ReaderTest() {
    }
    
    @BeforeAll
    public static void setUpClass() {
    }
    
    @AfterAll
    public static void tearDownClass() {
    }
    
    @BeforeEach
    public void setUp() {
    }
    
    @AfterEach
    public void tearDown() {
    }

    /**
     * Test of has_next method, of class Reader.
     */
    @Test
    public void testHas_next() {
        System.out.println("has_next");
        Reader instance = new Reader<>(basicHelloworld,DemoMessages.BasicMessage.parser());
        boolean expResult = true;
        boolean result = instance.has_next();
        assertEquals(expResult, result);
    }

    /**
     * Test of get_next method, of class Reader.
     */
    @Test
    public void testGet_next() {
        System.out.println("get_next");
        Reader instance = new Reader<>(basicHelloworld,DemoMessages.BasicMessage.parser());
        
        DemoMessages.BasicMessage result = (DemoMessages.BasicMessage)instance.get_next();
        assertEquals("helloworld", result.getMyString());
        assertEquals(0, result.getMyInt());
        
        // should get the same value back
        result = (DemoMessages.BasicMessage)instance.get_next();
        assertEquals("helloworld", result.getMyString());
        assertEquals(0, result.getMyInt());
    }

    /**
     * Test of take_next method, of class Reader.
     */
    @Test
    public void testTake_next() {
        System.out.println("take_next");
        Reader instance = new Reader<>(basicHelloworld,DemoMessages.BasicMessage.parser());
        
        DemoMessages.BasicMessage result = (DemoMessages.BasicMessage)instance.take_next();
        assertEquals("helloworld", result.getMyString());
        assertEquals(0, result.getMyInt());
        
        // should get the next value back
        result = (DemoMessages.BasicMessage)instance.take_next();
        assertEquals("helloworld", result.getMyString());
        assertEquals(1, result.getMyInt());
    }

    /**
     * Test of get_item method, of class Reader.
     */
    @Test
    public void testGet_item() {
        System.out.println("get_item");
        Reader instance = new Reader<>(basicHelloworld,DemoMessages.BasicMessage.parser());
        
        DemoMessages.BasicMessage result = (DemoMessages.BasicMessage)instance.get_item(1);
        assertEquals(1, result.getMyInt());
        
        result = (DemoMessages.BasicMessage)instance.get_item(5);
        assertEquals(5, result.getMyInt());
    }

    /**
     * Test of get_next_timestamp method, of class Reader.
     */
    @Test
    public void testGet_next_timestamp() {
        System.out.println("get_next_timestamp");
        Reader instance = new Reader<>(basicHelloworld,DemoMessages.BasicMessage.parser());
        long expResult = 0L;
        long result = instance.get_next_timestamp();
        assertEquals(expResult, result);
    }

    /**
     * Test of size method, of class Reader.
     */
    @Test
    public void testSize() {
        System.out.println("size");
        Reader instance = new Reader<>(basicHelloworld,DemoMessages.BasicMessage.parser());
        long expResult = 10L;
        long result = instance.size();
        assertEquals(expResult, result);
    }

    /**
     * Test of flags method, of class Reader.
     */
    @Test
    public void testFlags() {
        System.out.println("flags");
        Reader instance = new Reader<>(basicHelloworld,DemoMessages.BasicMessage.parser());
        int expResult = 0x1;// only valid bit should be set
        int result = instance.flags();
        assertEquals(expResult, result);
    }

    /**
     * Test of has_assumed_data method, of class Reader.
     */
    @Test
    public void testHas_assumed_data() {
        System.out.println("has_assumed_data");
        Reader instance = new Reader<>(basicHelloworld,DemoMessages.BasicMessage.parser());
        boolean expResult = false;
        boolean result = instance.has_assumed_data();
        assertEquals(expResult, result);
    }

    /**
     * Test of has_timestamps method, of class Reader.
     */
    @Test
    public void testHas_timestamps() {
        System.out.println("has_timestamps");
        Reader instance = new Reader<>(basicHelloworld,DemoMessages.BasicMessage.parser());
        boolean expResult = false;
        boolean result = instance.has_timestamps();
        assertEquals(expResult, result);
    }

    /**
     * Test of get_start_time method, of class Reader.
     */
    @Test
    public void testGet_start_time() {
        System.out.println("get_start_time");
        Reader instance = new Reader<>(basicHelloworld,DemoMessages.BasicMessage.parser());
        long result = instance.get_start_time();
        assert(result > 0);
    }

    /**
     * Test of get_version method, of class Reader.
     */
    @Test
    public void testGet_version() {
        System.out.println("get_version");
        Reader instance = new Reader<>(basicHelloworld,DemoMessages.BasicMessage.parser());
        Protorecord.Version result = instance.get_version();
        assertEquals(0, result.getMajor());
        assertEquals(2, result.getMinor());
        assertEquals(0, result.getPatch());
    }

    /**
     * Test of reason method, of class Reader.
     */
    @Test
    public void testReason() {
        System.out.println("reason");
        Reader instance = new Reader<>(basicHelloworld,DemoMessages.BasicMessage.parser());
        String expResult = "";
        String result = instance.reason();
        assertEquals(expResult, result);
    }

    /**
     * Test of close method, of class Reader.
     */
    @Test
    public void testClose() {
        System.out.println("close");
        Reader instance = new Reader<>(basicHelloworld,DemoMessages.BasicMessage.parser());
        instance.close();
    }
    
}
