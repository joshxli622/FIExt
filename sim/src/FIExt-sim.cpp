#include <pirate/tr/bar/BarSimRunner.hpp>
using pirate::tr::bar::BarSimRunner;

#include <FIExt-factory.hpp>

int main(int argc, char** argv) {
    Factory stratFactory;
    BarSimRunner sim("FIExt");
    return sim.runWithArgs(argc, argv, &stratFactory);
}
