import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

public class TestLogging {

 public static Logger logger = null;

 public static void setUpLogging() {
    System.setProperty("log4j.configurationFile",  "resources/log4j2.xml");
    logger = LogManager.getLogger(Logger.class.getName());  
 }

 public static void main(String[] args) {
    setUpLogging();
    logger.info("ao that day");
    logger.error("loi ne");
 }

}