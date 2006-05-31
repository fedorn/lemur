-------------------------------------------------------------------------------
$Id$
-------------------------------------------------------------------------------
0.0 Contents

1.0 Introduction
2.0 Webmasters!
3.0 System Runtime Requirements
4.0 Getting Started
5.0 Developer Documentation
6.0 Release History
7.0 License
8.0 Dependencies


1.0 Introduction
Heritrix is the Internet Archive's open-source, extensible, web-scale,
archival-quality web crawler project. Heritrix (sometimes spelled heretrix, or
misspelled or missaid as heratrix/heritix/heretix/heratix) is an archaic word
for heiress (woman who inherits). Since our crawler seeks to collect and
preserve the digital artifacts of our culture for the benefit of future
researchers and generations, this name seemed apt. 

2.0 Webmasters!
Heritrix is designed to respect the robots.txt 
<http://www.robotstxt.org/wc/robots.html> exclusion directives and META robots
tags <http://www.robotstxt.org/wc/exclusion.html#meta>. If you notice our
crawler behaving poorly, please send us email at archive-crawler-agent *at*
lists *dot* sourceforge *dot* net. 

3.0 System Runtime Requirements

3.1. Java Runtime Environment
The Heritrix crawler is implemented purely in java. This means that the only
true requirement for running it is that you have a JRE installed. The Heritrix
crawler makes use of Java 1.4 features so your JRE must be at least of a 1.4.0
pedigree. We currently include all of the free/open source third-party
libraries necessary to run Heritrix in the distribution package.  They are
listed along with pointers to their licenses in Section 8. Dependencies below.

3.2. Hardware
Default heap size is 256MB RAM.  This should be suitable for crawls that range
over hundreds of hosts. 

3.3. Linux
The Heritrix crawler has been built and tested primarily on Linux. It has seen
some informal use on Macintosh, Windows 2000 and Windows XP, but is not tested,
packaged, nor supported on platforms other than Linux at this time. 

4.0 Getting Started
See the User Manual at ./docs/articles/user_manual.html or at
<http://crawler.archive.org/articles/user_manual.html>.

5.0 Developer Documentation
See ./docs/articles/developer_manual.html or
<http://crawler.archive.org/articles/developer_manual.html>.


6.0 Release History

See the Heritrix Release Notes in the local directory
docs/articles/releasenotes.html if this is a binary release or
at http://crawler.archive.org/articles/releasenotes.html.


7.0 License

Heritrix is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser Public License as published by the
Free Software Foundation; either version 2.1 of the License, or any
later version.
                                                                                
Heritrix is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser Public License for more details.
                                                                                
You should have received a copy of the GNU Lesser Public License
along with Heritrix (See LICENSE.txt); if not, write to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
02111-1307  USA
                                                                                
For the licenses for libraries used by Heritrix and included in its
distribution, see below in section '8.0 Dependencies'.


8.0 Dependencies

8.1. bdb
Version: 1.7.1
Url: http://www.sleepycat.com/products/je.shtml
Description: Berkeley DB Java Edition. Copyright (c) 1990-2004 Sleepycat
Software. All rights reserved. See above link for how to obtain source.
License: http://www.sleepycat.com/download/jeoslicense.html

8.2. commons-httpclient
Version: 3.0-beta1
Url: http://jakarta.apache.org/commons/httpclient/
Description: This package is used for fetching URIs via http.
License: Apache 2.0 http://www.apache.org/licenses/LICENSE-2.0

8.3. commons-logging
Version: 1.0.4
Url: http://jakarta.apache.org/commons/logging.html
Description: Provides logging adapters. 
License: Apache 2.0 http://www.apache.org/licenses/LICENSE-2.0

8.4. commons-codec
Version: 1.3
Url: http://jakarta.apache.org/commons/codec/
Description: Commons Codec provides implementations of common encoders and
decoders such as Base64, Hex, various phonetic encodings, and URLs.
License: Apache 2.0 http://www.apache.org/licenses/LICENSE-2.0

8.5. dnsjava
Version: 1.6.2
Url: http://www.dnsjava.org/
Description: DNS Lookups.
License: BSD

8.6. jetty
Version: 4.2.23
Url: http://jetty.mortbay.com/jetty/
Description: The Jetty servlet container.
License: Jetty license, http://jetty.mortbay.org/jetty/LICENSE.html

8.7. servlet
Version: 2.3
Url: http://jakarta.apache.org/tomcat/
Description: Taken from tomcat.
License: http://jakarta.apache.org/site/legal.html

8.8. jasper-runtime
Version: 4.1.30
Url: http://jakarta.apache.org/tomcat/
Description: Taken from tomcat.
License: http://jakarta.apache.org/site/legal.html

8.9. jasper-compiler
Version: 4.1.30
Url: http://jakarta.apache.org/tomcat/
Description: Taken from tomcat.
License: http://jakarta.apache.org/site/legal.html

8.10. jmxri
Version: 
Url: http://java.sun.com/products/JavaManagement/index.jsp
Description: JMX Reference Implementation.
License: SUN Binary Code License http://java.com/en/download/license.jsp

8.11. jmxtools
Version: 
Url: http://java.sun.com/products/JavaManagement/index.jsp
Description: JMX tools.
License: SUN Binary Code License http://java.com/en/download/license.jsp

8.12. poi
Version: 2.0-RC1-20031102
Url: http://jakarta.apache.org/poi/
Description: For parsing PDFs.
License: Apache 1.1 http://www.apache.org/LICENSE.txt

8.13. poi-scratchpad
Version: 2.0-RC1-20031102
Url: http://jakarta.apache.org/poi/
Description: For parsing PDFs. Has the
org.apache.poi.hdf.extractor.WordDocument.
License: Apache 1.1 http://www.apache.org/LICENSE.txt

8.14. javaswf
Version: 
Url: http://www.anotherbigidea.com/javaswf
Description: JavaSWF2 is a set of Java packages that enable the parsing,
manipulation and generation of the Macromedia Flash(TM) file format known as
SWF ("swiff"). Added jar was made by unzipping javaswf-CVS-SNAPSHOT-1.zip
download, compiling the java classes therein, and then making a jar of the
product.
License: The JavaSWF BSD License, http://anotherbigidea.com/javaswf/JavaSWF2-BSD.LICENSE.html

8.15. itext
version: 1.2
url: http://www.lowagie.com/iText/
truedescription: A library for parsing PDF files.
license: MPL (http://www.lowagie.com/iText/MPL-1.1.txt)

8.16. ant
Version: 1.6.2
Url: http://ant.apache.org
Description: Build tool. An ant task is used to compile the jspc pages at
build time and then for the selftest at runtime.
License: Apache 1.1. http://ant.apache.org/license.html

8.17. junit
Version: 3.8.1
Url: http://www.junit.org/
Description: A framework for implimenting the unit testing methology. 
License: IBM's Common Public License Version 0.5.

8.18. commons-pool
Version: 1.2
Url: http://jakarta.apache.org/site/binindex.cgi#commons-pool
Description: For object pooling.
License: Apache 1.1 http://www.apache.org/LICENSE.txt

8.19. commons-collections
Version: 3.1
Url: http://jakarta.apache.org/site/binindex.cgi#commons-collections
Description: Needed by commons-pool.
License: Apache 1.1 http://www.apache.org/LICENSE.txt

8.20. commons-cli
Version: 1.0
Url: http://jakarta.apache.org/site/binindex.cgi
Description: Needed doing Heritrix command-line processing.
License: Apache 1.1 http://www.apache.org/LICENSE.txt

8.21. concurrent
Version: 1.3.2
Url: http://gee.cs.oswego.edu/dl/classes/EDU/oswego/cs/dl/util/concurrent/intro.html
Description: Concurrency utilities.
License: Public Domain

8.22. commons-net
Version: 1.1.0
Url: http://jakarta.apache.org/commons/net/
Description: This is an Internet protocol suite Java library originally 
developed by ORO, Inc. This version supports Finger, Whois, TFTP, Telnet,
POP3, FTP, NNTP, SMTP, and some miscellaneous protocols like Time and Echo as
well as BSD R command support. Heritrix uses its FTP implementation.
License: Apache 1.1 http://www.apache.org/LICENSE.txt

8.23. dsi-unimi-it
Version: 0.9.1
Url: http://mg4j.dsi.unimi.it/
Description: This JAR supplies alternatives to String, StringBuffer, and
unsynchronized I/0. This JAR was made from subsets of mg4j-0.9.1 and from
fastutil-4.4.0 -- two jars that came out of the ubicrawler project,
http://ubi0.iit.cnr.it/projects/ubi/ -- using autojar. Here is how I made this
jar: % java -jar autojar-1.2.2/autojar-1.2.2.jar -v -o \
    dsi.unimi.it-mg4j-0.9.1_fastutil-4.4.0.jar -c \
    mg4j-0.9.1/mg4j-0.9.1.jar:fastutil-4.4.0/fastutil-4.4.0.jar it.unimi.dsi.mg4j.util.MutableString.class it.unimi.dsi.mg4j.io.FastBufferedInputStream.class it.unimi.dsi.mg4j.io.FastBufferedOutputStream.class it.unimi.dsi.mg4j.io.FastBufferedReader.class it.unimi.dsi.mg4j.io.FastByteArrayInputStream.class it.unimi.dsi.mg4j.io.FastByteArrayOutputStream.class it.unimi.dsi.mg4j.io.FastMultiByteArrayInputStream.class
License: Both MG4J and fastutils are LGPL
