#include "gtest/gtest.h"
#include "sclap.h"

/*////////////////////////////////////////////////////////////////////////////////////////////////*/

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    getchar();
    return 0;
}

/*////////////////////////////////////////////////////////////////////////////////////////////////*/

TEST(SingleReadTest, TestBool1)
{
    int argc = 2;
    char* argv[2];
    argv[0] = "Program Name";
    argv[1] = "-t=true";

    sclap::OptionDescriptors descriptors;
    descriptors << sclap::OptionDescriptor('t', "", sclap::ARG_BOOL);

    sclap::Options options(descriptors, argc, argv);
    EXPECT_TRUE(descriptors.valid());
    EXPECT_TRUE(options.valid());

    EXPECT_TRUE(options['t']);
    EXPECT_TRUE(options['t'].asBool());
    EXPECT_EQ(options['t'].asString(), "true");

    EXPECT_EQ(options['t'].type(), sclap::ARG_BOOL);
}

/*////////////////////////////////////////////////////////////////////////////////////////////////*/

TEST(SingleReadTest, TestBool2)
{
    int argc = 3;
    char* argv[3];
    argv[0] = "Program Name";
    argv[1] = "-t";
    argv[2] = "false";

    sclap::OptionDescriptors descriptors;
    descriptors << sclap::OptionDescriptor('t', "", sclap::ARG_BOOL);

    sclap::Options options(descriptors, argc, argv);
    EXPECT_TRUE(descriptors.valid());
    EXPECT_TRUE(options.valid());

    EXPECT_FALSE(options['t']);
    EXPECT_FALSE(options['t'].asBool());
}

/*////////////////////////////////////////////////////////////////////////////////////////////////*/

TEST(SingleReadTest, TestBool3)
{
    int argc = 2;
    char* argv[2];
    argv[0] = "Program Name";
    argv[1] = "--test";

    sclap::OptionDescriptors descriptors;
    descriptors << sclap::OptionDescriptor('t', "test", sclap::ARG_BOOL);

    sclap::Options options(descriptors, argc, argv);
    EXPECT_TRUE(descriptors.valid());
    EXPECT_TRUE(options.valid());

    EXPECT_TRUE(options["test"].asBool());
    EXPECT_TRUE(options["test"]);
}

/*////////////////////////////////////////////////////////////////////////////////////////////////*/

TEST(SingleReadTest, TestInt)
{
    int argc = 3;
    char* argv[3];
    argv[0] = "Program Name";
    argv[1] = "--test";
    argv[2] = "100";

    sclap::OptionDescriptors descriptors;
    descriptors << sclap::OptionDescriptor('t', "test", sclap::ARG_INT);

    sclap::Options options(descriptors, argc, argv);
    EXPECT_TRUE(descriptors.valid());
    EXPECT_TRUE(options.valid());

    EXPECT_TRUE(options["test"].asBool());
    EXPECT_EQ(options["test"].asInteger(), 100);
    EXPECT_EQ(options["test"].asDouble(), 100);
    EXPECT_EQ(options["test"].asString(), "100");

    EXPECT_EQ(options['t'].type(), sclap::ARG_INT);
}

/*////////////////////////////////////////////////////////////////////////////////////////////////*/

TEST(SingleReadTest, TestDouble)
{
    int argc = 3;
    char* argv[3];
    argv[0] = "Program Name";
    argv[1] = "--test";
    argv[2] = "100.25";

    sclap::OptionDescriptors descriptors;
    descriptors << sclap::OptionDescriptor('t', "test", sclap::ARG_REAL);

    sclap::Options options(descriptors, argc, argv);
    EXPECT_TRUE(descriptors.valid());
    EXPECT_TRUE(options.valid());

    EXPECT_TRUE(options["test"].asBool());
    EXPECT_EQ(options["test"].asInteger(), 100);
    EXPECT_EQ(options["test"].asDouble(), 100.25);
    EXPECT_EQ(options["test"].asString(), "100.25");

    EXPECT_EQ(options['t'].type(), sclap::ARG_REAL);
}

/*////////////////////////////////////////////////////////////////////////////////////////////////*/

TEST(SingleReadTest, TestString)
{
    int argc = 3;
    char* argv[3];
    argv[0] = "Program Name";
    argv[1] = "--test";
    argv[2] = "100.25";

    sclap::OptionDescriptors descriptors;
    descriptors << sclap::OptionDescriptor('t', "test", sclap::ARG_STRING);

    sclap::Options options(descriptors, argc, argv);
    EXPECT_TRUE(descriptors.valid());
    EXPECT_TRUE(options.valid());

    EXPECT_TRUE(options["test"].asBool());
    EXPECT_EQ(options["test"].asString(), "100.25");

    EXPECT_EQ(options['t'].type(), sclap::ARG_STRING);
}

/*////////////////////////////////////////////////////////////////////////////////////////////////*/

TEST(SingleReadTest, TestBoolVector)
{
    int argc = 3;
    char* argv[3];
    argv[0] = "Program Name";
    argv[1] = "--test";
    argv[2] = "false";

    sclap::OptionDescriptors descriptors;
    descriptors << sclap::OptionDescriptor('t', "test", sclap::ARG_BOOL_VEC);

    sclap::Options options(descriptors, argc, argv);
    EXPECT_TRUE(descriptors.valid());
    EXPECT_TRUE(options.valid());

    EXPECT_FALSE(options["test"]);
    EXPECT_FALSE(options["test"].asBool());
    EXPECT_FALSE(options["test"].asBoolVector().at(0));
    EXPECT_EQ(options['t'].type(), sclap::ARG_BOOL_VEC);
}

/*////////////////////////////////////////////////////////////////////////////////////////////////*/

TEST(SingleReadTest, TestIntVector)
{
    int argc = 3;
    char* argv[3];
    argv[0] = "Program Name";
    argv[1] = "--test";
    argv[2] = "12";

    sclap::OptionDescriptors descriptors;
    descriptors << sclap::OptionDescriptor('t', "test", sclap::ARG_INT_VEC);
    sclap::Options options(descriptors, argc, argv);
    EXPECT_TRUE(descriptors.valid());
    EXPECT_TRUE(options.valid());

    EXPECT_TRUE(options["test"]);
    EXPECT_TRUE(options["test"].asBool());
    EXPECT_EQ(options["test"].asIntegerVector().at(0), 12);
    EXPECT_EQ(options['t'].type(), sclap::ARG_INT_VEC);
}

/*////////////////////////////////////////////////////////////////////////////////////////////////*/

TEST(SingleReadTest, TestDoubleVector)
{
    int argc = 3;
    char* argv[3];
    argv[0] = "Program Name";
    argv[1] = "--test";
    argv[2] = "12.375641876";

    sclap::OptionDescriptors descriptors;
    descriptors << sclap::OptionDescriptor('t', "test", sclap::ARG_REAL_VEC);

    sclap::Options options(descriptors, argc, argv);
    EXPECT_TRUE(descriptors.valid());
    EXPECT_TRUE(options.valid());

    EXPECT_TRUE(options["test"]);
    EXPECT_TRUE(options["test"].asBool());
    EXPECT_EQ(options["test"].asRealVector().at(0), 12.375641876);
    EXPECT_EQ(options['t'].type(), sclap::ARG_REAL_VEC);
}

/*////////////////////////////////////////////////////////////////////////////////////////////////*/

TEST(SingleReadTest, TestStringVector)
{
    int argc = 3;
    char* argv[3];
    argv[0] = "Program Name";
    argv[1] = "--test";
    argv[2] = "12.375641876a";

    sclap::OptionDescriptors descriptors;
    descriptors << sclap::OptionDescriptor('t', "test", sclap::ARG_STRING_VEC);

    sclap::Options options(descriptors, argc, argv);
    EXPECT_TRUE(descriptors.valid());
    EXPECT_TRUE(options.valid());

    EXPECT_TRUE(options["test"]);
    EXPECT_TRUE(options["test"].asBool());
    EXPECT_EQ(options["test"].asStringVector().at(0), "12.375641876a");
    EXPECT_EQ(options['t'].type(), sclap::ARG_STRING_VEC);
}

/*////////////////////////////////////////////////////////////////////////////////////////////////*/

TEST(DoubleReadTest, TestBool1)
{
    int argc = 3;
    char* argv[3];
    argv[0] = "Program Name";
    argv[1] = "-t=true";
    argv[2] = "-e=false";

    sclap::OptionDescriptors descriptors;
    descriptors << sclap::OptionDescriptor('t', "", sclap::ARG_BOOL)
                << sclap::OptionDescriptor('e', "", sclap::ARG_BOOL);

    sclap::Options options(descriptors, argc, argv);
    EXPECT_TRUE(descriptors.valid());
    EXPECT_TRUE(options.valid());

    EXPECT_TRUE(options['t']);
    EXPECT_TRUE(options['t'].asBool());
    EXPECT_EQ(options['t'].asString(), "true");

    EXPECT_FALSE(options['e']);
    EXPECT_FALSE(options['e'].asBool());
    EXPECT_EQ(options['e'].asString(), "false");

    EXPECT_EQ(options['t'].type(), sclap::ARG_BOOL);
    EXPECT_EQ(options['e'].type(), sclap::ARG_BOOL);
}

/*////////////////////////////////////////////////////////////////////////////////////////////////*/

TEST(DoubleReadTest, TestBool2)
{
    int argc = 5;
    char* argv[5];
    argv[0] = "Program Name";
    argv[1] = "-t";
    argv[2] = "false";
    argv[3] = "-e";
    argv[4] = "false";

    sclap::OptionDescriptors descriptors;
    descriptors << sclap::OptionDescriptor('t', "", sclap::ARG_BOOL)
                << sclap::OptionDescriptor('e', "", sclap::ARG_BOOL);

    sclap::Options options(descriptors, argc, argv);
    EXPECT_TRUE(descriptors.valid());
    EXPECT_TRUE(options.valid());

    EXPECT_FALSE(options['t']);
    EXPECT_FALSE(options['t'].asBool());

    EXPECT_FALSE(options['e']);
    EXPECT_FALSE(options['e'].asBool());
}

/*////////////////////////////////////////////////////////////////////////////////////////////////*/

TEST(DoubleReadTest, TestBool3)
{
    int argc = 3;
    char* argv[3];
    argv[0] = "Program Name";
    argv[1] = "--test";
    argv[2] = "--est";

    sclap::OptionDescriptors descriptors;
    descriptors << sclap::OptionDescriptor('t', "test", sclap::ARG_BOOL)
                << sclap::OptionDescriptor('t', "est", sclap::ARG_BOOL);

    sclap::Options options(descriptors, argc, argv);
    EXPECT_TRUE(descriptors.valid());
    EXPECT_TRUE(options.valid());

    EXPECT_TRUE(options["test"].asBool());
    EXPECT_TRUE(options["test"]);

    EXPECT_TRUE(options["est"].asBool());
    EXPECT_TRUE(options["est"]);
}

/*////////////////////////////////////////////////////////////////////////////////////////////////*/

TEST(DoubleReadTest, TestInt)
{
    int argc = 5;
    char* argv[5];
    argv[0] = "Program Name";
    argv[1] = "--test";
    argv[2] = "100";
    argv[3] = "--est";
    argv[4] = "101";

    sclap::OptionDescriptors descriptors;
    descriptors << sclap::OptionDescriptor('t', "test", sclap::ARG_INT)
                << sclap::OptionDescriptor('e', "est", sclap::ARG_INT);

    sclap::Options options(descriptors, argc, argv);
    EXPECT_TRUE(descriptors.valid());
    EXPECT_TRUE(options.valid());

    EXPECT_TRUE(options["test"].asBool());
    EXPECT_EQ(options["test"].asInteger(), 100);
    EXPECT_EQ(options["test"].asDouble(), 100);
    EXPECT_EQ(options["test"].asString(), "100");

    EXPECT_TRUE(options["est"].asBool());
    EXPECT_EQ(options["est"].asInteger(), 101);
    EXPECT_EQ(options["est"].asDouble(), 101);
    EXPECT_EQ(options["est"].asString(), "101");

    EXPECT_EQ(options['t'].type(), sclap::ARG_INT);
    EXPECT_EQ(options['e'].type(), sclap::ARG_INT);
}

/*////////////////////////////////////////////////////////////////////////////////////////////////*/

TEST(DoubleReadTest, TestDouble)
{
    int argc = 5;
    char* argv[5];
    argv[0] = "Program Name";
    argv[1] = "--test";
    argv[2] = "100.25";
    argv[3] = "--est";
    argv[4] = "101.25";

    sclap::OptionDescriptors descriptors;
    descriptors << sclap::OptionDescriptor('t', "test", sclap::ARG_REAL)
                << sclap::OptionDescriptor('e', "est", sclap::ARG_REAL);

    sclap::Options options(descriptors, argc, argv);
    EXPECT_TRUE(descriptors.valid());
    EXPECT_TRUE(options.valid());

    EXPECT_TRUE(options["test"].asBool());
    EXPECT_EQ(options["test"].asInteger(), 100);
    EXPECT_EQ(options["test"].asDouble(), 100.25);
    EXPECT_EQ(options["test"].asString(), "100.25");

    EXPECT_TRUE(options["est"].asBool());
    EXPECT_EQ(options["est"].asInteger(), 101);
    EXPECT_EQ(options["est"].asDouble(), 101.25);
    EXPECT_EQ(options["est"].asString(), "101.25");

    EXPECT_EQ(options['t'].type(), sclap::ARG_REAL);
    EXPECT_EQ(options['e'].type(), sclap::ARG_REAL);
}

/*////////////////////////////////////////////////////////////////////////////////////////////////*/

TEST(DoubleReadTest, TestString)
{
    int argc = 5;
    char* argv[5];
    argv[0] = "Program Name";
    argv[1] = "--test";
    argv[2] = "100.25";
    argv[3] = "--est";
    argv[4] = "101.25";

    sclap::OptionDescriptors descriptors;
    descriptors << sclap::OptionDescriptor('t', "test", sclap::ARG_STRING)
                << sclap::OptionDescriptor('e', "est", sclap::ARG_STRING);

    sclap::Options options(descriptors, argc, argv);
    EXPECT_TRUE(descriptors.valid());
    EXPECT_TRUE(options.valid());

    EXPECT_TRUE(options["test"].asBool());
    EXPECT_EQ(options["test"].asString(), "100.25");
 
    EXPECT_TRUE(options["est"].asBool());
    EXPECT_EQ(options["est"].asString(), "101.25");

    EXPECT_EQ(options['t'].type(), sclap::ARG_STRING);
    EXPECT_EQ(options['e'].type(), sclap::ARG_STRING);
}

/*////////////////////////////////////////////////////////////////////////////////////////////////*/

TEST(DoubleReadTest, TestBoolVector)
{
    int argc = 7;
    char* argv[7];
    argv[0] = "Program Name";
    argv[1] = "--test";
    argv[2] = "false";
    argv[3] = "true";
    argv[4] = "--est";
    argv[5] = "false";
    argv[6] = "true";

    sclap::OptionDescriptors descriptors;
    descriptors << sclap::OptionDescriptor('t', "test", sclap::ARG_BOOL_VEC)
                << sclap::OptionDescriptor('e', "est", sclap::ARG_BOOL_VEC);

    sclap::Options options(descriptors, argc, argv);
    EXPECT_TRUE(descriptors.valid());
    EXPECT_TRUE(options.valid());

    EXPECT_FALSE(options["test"]);
    EXPECT_FALSE(options["test"].asBool());
    EXPECT_FALSE(options["test"].asBoolVector().at(0));
    EXPECT_TRUE(options["test"].asBoolVector().at(1));

    EXPECT_FALSE(options["est"]);
    EXPECT_FALSE(options["est"].asBool());
    EXPECT_FALSE(options["est"].asBoolVector().at(0));
    EXPECT_TRUE(options["est"].asBoolVector().at(1));

    EXPECT_EQ(options['t'].type(), sclap::ARG_BOOL_VEC);
    EXPECT_EQ(options['e'].type(), sclap::ARG_BOOL_VEC);
}

/*////////////////////////////////////////////////////////////////////////////////////////////////*/

TEST(DoubleReadTest, TestIntVector)
{
    int argc = 7;
    char* argv[7];
    argv[0] = "Program Name";
    argv[1] = "--test";
    argv[2] = "12";
    argv[3] = "13";
    argv[4] = "--est";
    argv[5] = "12";
    argv[6] = "13";

    sclap::OptionDescriptors descriptors;
    descriptors << sclap::OptionDescriptor('t', "test", sclap::ARG_INT_VEC)
                << sclap::OptionDescriptor('e', "est", sclap::ARG_INT_VEC);

    sclap::Options options(descriptors, argc, argv);
    EXPECT_TRUE(descriptors.valid());
    EXPECT_TRUE(options.valid());

    EXPECT_TRUE(options["test"]);
    EXPECT_TRUE(options["test"].asBool());
    EXPECT_EQ(options["test"].asIntegerVector().at(0), 12);
    EXPECT_EQ(options["test"].asIntegerVector().at(1), 13);

    EXPECT_TRUE(options["est"]);
    EXPECT_TRUE(options["est"].asBool());
    EXPECT_EQ(options["est"].asIntegerVector().at(0), 12);
    EXPECT_EQ(options["est"].asIntegerVector().at(1), 13);

    EXPECT_EQ(options['t'].type(), sclap::ARG_INT_VEC);
    EXPECT_EQ(options['e'].type(), sclap::ARG_INT_VEC);
}

/*////////////////////////////////////////////////////////////////////////////////////////////////*/

TEST(DoubleReadTest, TestDoubleVector)
{
    int argc = 7;
    char* argv[7];
    argv[0] = "Program Name";
    argv[1] = "--test";
    argv[2] = "12.375641876";
    argv[3] = "1.375641876";
    argv[4] = "--est";
    argv[5] = "12.375641876";
    argv[6] = "1.375641876";

    sclap::OptionDescriptors descriptors;
    descriptors << sclap::OptionDescriptor('t', "test", sclap::ARG_REAL_VEC)
                << sclap::OptionDescriptor('e', "est", sclap::ARG_REAL_VEC);

    sclap::Options options(descriptors, argc, argv);
    EXPECT_TRUE(descriptors.valid());
    EXPECT_TRUE(options.valid());

    EXPECT_TRUE(options["test"]);
    EXPECT_TRUE(options["test"].asBool());
    EXPECT_EQ(options["test"].asRealVector().at(0), 12.375641876);
    EXPECT_EQ(options["test"].asRealVector().at(1), 1.375641876);

    EXPECT_TRUE(options["est"]);
    EXPECT_TRUE(options["est"].asBool());
    EXPECT_EQ(options["est"].asRealVector().at(0), 12.375641876);
    EXPECT_EQ(options["est"].asRealVector().at(1), 1.375641876);

    EXPECT_EQ(options['t'].type(), sclap::ARG_REAL_VEC);
    EXPECT_EQ(options['e'].type(), sclap::ARG_REAL_VEC);
}

/*////////////////////////////////////////////////////////////////////////////////////////////////*/

TEST(DoubleReadTest, TestStringVector)
{
    int argc = 7;
    char* argv[7];
    argv[0] = "Program Name";
    argv[1] = "--test";
    argv[2] = "12.375641876a";
    argv[3] = "1.375641876a";
    argv[4] = "--est";
    argv[5] = "12.375641876a";
    argv[6] = "1.375641876a";

    sclap::OptionDescriptors descriptors;
    descriptors << sclap::OptionDescriptor('t', "test", sclap::ARG_STRING_VEC)
        << sclap::OptionDescriptor('e', "est", sclap::ARG_STRING_VEC);

    sclap::Options options(descriptors, argc, argv);
    EXPECT_TRUE(descriptors.valid());
    EXPECT_TRUE(options.valid());

    EXPECT_TRUE(options["test"]);
    EXPECT_TRUE(options["test"].asBool());
    EXPECT_EQ(options["test"].asStringVector().at(0), "12.375641876a");
    EXPECT_EQ(options["test"].asStringVector().at(1), "1.375641876a");

    EXPECT_TRUE(options["est"]);
    EXPECT_TRUE(options["est"].asBool());
    EXPECT_EQ(options["est"].asStringVector().at(0), "12.375641876a");
    EXPECT_EQ(options["est"].asStringVector().at(1), "1.375641876a");

    EXPECT_EQ(options['t'].type(), sclap::ARG_STRING_VEC);
    EXPECT_EQ(options['e'].type(), sclap::ARG_STRING_VEC);
}

/*////////////////////////////////////////////////////////////////////////////////////////////////*/

TEST(RandomReadTest, Random1)
{
    int argc = 3;
    char* argv[3];
    argv[0] = "Program Name";
    argv[1] = "-defi";
    argv[2] = "demotest";

    sclap::OptionDescriptors descriptors;
    descriptors << sclap::OptionDescriptor('d', "download", sclap::ARG_STRING_VEC)
                << sclap::OptionDescriptor('e', "execute", sclap::ARG_STRING)
                << sclap::OptionDescriptor('f', "friendly", sclap::ARG_BOOL)
                << sclap::OptionDescriptor('i', "interactive", sclap::ARG_BOOL);

    sclap::Options options(descriptors, argc, argv);
    EXPECT_TRUE(descriptors.valid());
    EXPECT_TRUE(options.valid());

    EXPECT_TRUE(options["d"]);
    EXPECT_TRUE(options["e"]);
    EXPECT_TRUE(options["f"]);
    EXPECT_TRUE(options["i"]);

    EXPECT_TRUE(options["download"].asBool());
    EXPECT_TRUE(options["execute"].asBool());
    EXPECT_TRUE(options["friendly"].asBool());
    EXPECT_TRUE(options["interactive"].asBool());

    EXPECT_EQ(options["download"].asStringVector().at(0), "demotest");
    EXPECT_EQ(options["download"].asString(), "demotest");

    EXPECT_EQ(options["execute"].asStringVector().at(0), "demotest");
    EXPECT_EQ(options["execute"].asString(), "demotest");

    EXPECT_EQ(options['d'].type(), sclap::ARG_STRING_VEC);
    EXPECT_EQ(options['e'].type(), sclap::ARG_STRING_VEC);
    EXPECT_EQ(options['f'].type(), sclap::ARG_STRING_VEC);
    EXPECT_EQ(options['i'].type(), sclap::ARG_STRING_VEC);
}

/*////////////////////////////////////////////////////////////////////////////////////////////////*/