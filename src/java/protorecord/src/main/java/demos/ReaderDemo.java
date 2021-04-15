/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package demos;

import protorecord.Reader;
import protorecord.demo.DemoMessages;

/**
 *
 * @author daniel
 */
public class ReaderDemo {

    public static void main(String args[]) {
        Reader reader = new Reader<DemoMessages.BasicMessage>("/home/daniel/Downloads/recording",DemoMessages.BasicMessage.parser());
        System.out.printf("reason = %s\n", reader.reason());
        System.out.printf("There are %d items in this recording\n", reader.size());
    }
    
}
