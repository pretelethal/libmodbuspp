#include <iostream>
#include <modbuspp.h>
#include <UnitTest++/UnitTest++.h>

using namespace std;
using namespace Modbus;

SUITE(HoldingRegisterReadTest)
{
  TEST (RtuTcpHoldingRegisterReadTest) {
    string connection ("127.0.0.1");
    string settings ("1525");
    Net net = Tcp;
    int slvAddr = 1;

    Master mb;
    CHECK (!mb.isValid());

    CHECK (mb.setBackend (net, connection, settings));
    CHECK (mb.isValid());
    CHECK (mb.net() == net);
    CHECK (mb.connection() == connection);
    CHECK (mb.settings() == settings) ;

    Slave & slv = mb.addSlave (slvAddr);
    CHECK (mb.hasSlave (slvAddr));

    REQUIRE CHECK (mb.open ());
    uint16_t value;

    CHECK (slv.readRegisters (1, &value) == 1);

    cout << "R0=" << value << endl;
    mb.close();
    CHECK (!mb.isOpen());
  }

  TEST (RtuEncHoldingRegisterReadTest) {
    string connection ("127.0.0.1");
    string settings ("1526");
    Net net = Enc;
    int slvAddr = 1;

    Master mb;
    CHECK (!mb.isValid());

    CHECK (mb.setBackend (net, connection, settings));
    CHECK (mb.isValid());
    CHECK (mb.net() == net);
    CHECK (mb.connection() == connection);
    CHECK (mb.settings() == settings) ;

    Slave & slv = mb.addSlave (slvAddr);
    CHECK (mb.hasSlave (slvAddr));

    REQUIRE CHECK (mb.open ());
    uint16_t value;

    CHECK (slv.readRegisters (1, &value) == 1);

    cout << "R0=" << value << endl;
    mb.close();
    CHECK (!mb.isOpen());
  }
}

// run all tests
int main (int argc, char **argv) {
  return UnitTest::RunAllTests();
}
