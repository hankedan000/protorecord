/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package demos;

import java.io.File;
import protorecord.Reader;
import protorecord.demo.DemoMessages;

/**
 *
 * @author daniel
 */
public class ReaderDemo {

    public static void main(String args[]) {
        File recordPath = new File("/home/daniel/Downloads/recording");
        Reader reader = new Reader<DemoMessages.BasicMessage>(recordPath,DemoMessages.BasicMessage.parser());
        
        while (reader.has_next()) {
            DemoMessages.BasicMessage msg = (DemoMessages.BasicMessage)reader.take_next();
            System.out.println(msg.toString());
        }
    }
    
}
