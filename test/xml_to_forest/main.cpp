/*
    Copyright 2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://opensource.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <exception>
#include <iostream>

#include <adobe/xml_element_forest.hpp>

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

const char* const test_document_1 =
    "<math-test>\n"
    "   <expression>\n"
    "       <operand value=\"15\"/>\n"
    "       <operand value=\"10\"/>\n"
    "       <add/>\n"
    "   </expression>\n"
    "   <result value=\"25\"/>\n"
    "</math-test>";

const char* const test_document_2 =
    "<math-test>/n"
    "   <expression>/n"
    "       <operand value=\"3\"/>/n"
    "       <operand value=\"5\"/>/n"
    "       <multiply/>/n"
    "   </expression>/n"
    "   <result value=\"15\"/>/n"
    "</math-test>/n";

const char* const test_document_3 = 
    "<math-test>\n"
    "   <expression>\n"
    "       <operand value=\"2\"/>\n"
    "       <operand value=\"3\"/>\n"
    "       <expression>\n"
    "           <operand value=\"4\"/>\n"
    "           <operand value=\"5\"/>\n"
    "           <multiply/>\n"
    "       </expression>\n"
    "       <add/>\n"
    "       <multiply/>\n"
    "   </expression>\n"
    "   <result value=\"46\"/>\n"
    "</math-test>\n";

const char* const test_document_4 = 
    "<sport>"
    "    <basketball>"
    "        <division name='west'>"
    "            <team city='Los Angeles'>"
    "                Lakers"
    "            </team>"
    "        </division>"
    "        <division name='east'>"
    "            <team city='Chicago'>"
    "                Bulls"
    "            </team>"
    "        </division>"
    "    </basketball>"
    "    <baseball>"
    "        <league name='national'>"
    "            <team city='San Diego'>"
    "                Padres"
    "            </team>"
    "        </league>"
    "        <league name='american'>"
    "            <team city='Oakland'>"
    "                Athletics"
    "            </team>"
    "        </league>"
    "    </baseball>"
    "</sport>";

const char* const test_document_5 = 
    "<html>"
    "    <head>"
    "        <title>Sample XML-Compliant HTML Page</title>"
    "    </head>"
    "    <body>"
    "        This is free-floating text."
    "        <div align='center'>This is inside a div</div>"
    "        <p>A Paragraph</p>"
    "        <div>I'm <em>emphatically</em> em<b>bold</b>ened about this statement.</div>"
    "    </body>"
    "</html>";

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

int main()
try
{
    //adobe::element_forest_to_xml(adobe::depth_range(adobe::xml_parse_to_forest(test_document_1)), std::cout);

    //adobe::element_forest_to_xml(adobe::depth_range(adobe::xml_parse_to_forest(test_document_2)), std::cout);

    adobe::element_forest_to_xml(adobe::depth_range(adobe::xml_parse_to_forest(test_document_3)), std::cout);

    adobe::element_forest_to_xml(adobe::depth_range(adobe::xml_parse_to_forest(test_document_4)), std::cout);

    adobe::element_forest_to_xml(adobe::depth_range(adobe::xml_parse_to_forest(test_document_5)), std::cout);

    adobe::element_forest_to_xml(adobe::depth_range(adobe::xml_parse_to_forest(test_document_5)), std::cout, false);

    std::cout << std::endl;

    return 0;
}
catch(const std::exception& error)
{
    std::cerr << "Exception: " << error.what() << std::endl;

    return 1;
}
catch(...)
{
    std::cerr << "Exception" << std::endl;

    return 1;
}

/****************************************************************************************************/
