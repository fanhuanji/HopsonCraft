#include "Noise_Generator.h"

#include <cmath>

#include "../World/World_Constants.h"

namespace Noise
{
    uint64_t seed;

    uint64_t getSeed()
    {
        return seed;
    }

    void setSeed(uint64_t newSeed)
    {
        seed = newSeed;
    }

    double Generator::findNoise1(int n) const
    {
        n += m_seed;
        n = (n << 13) ^ n;
        auto nn = (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff; //wot
        return 1.0 - ((double)nn / 1073741824.0); //wot (part 2)
    }

    double Generator::findNoise2(double x, double z) const
    {
        return findNoise1(x + z * 57);
    }

    double Generator::lerp(double a, double b, double z) const
    {
        double mu2 = (1 - std::cos(z * 3.14)) / 2;
        return (a * (1 - mu2) + b * mu2);
    }

    double Generator::noise(double x, double z) const
    {
        auto floorX = (double)((int)x); //This is kinda a cheap way to floor a double integer.
        auto floorZ = (double)((int)z);

        auto    s = 0.0,
                t = 0.0,
                u = 0.0,
                v = 0.0;//Integer declaration

        s = findNoise2(floorX,      floorZ);
        t = findNoise2(floorX + 1,  floorZ);
        u = findNoise2(floorX,      floorZ + 1);//Get the surrounding values to calculate the transition.
        v = findNoise2(floorX + 1,  floorZ + 1);

        auto rec1 = lerp(s, t, x - floorX);//Interpolate between the values.
        auto rec2 = lerp(u, v, x - floorX);//Here we use x-floorX, to get 1st dimension. Don't mind the x-floorX thingie, it's part of the cosine formula.
        auto rec3 = lerp(rec1, rec2, z - floorZ);//Here we use y-floorZ, to get the 2nd dimension.
        return rec3;
    }

    double Generator::getValue(int x, int z, int tileX, int tileZ) const
    {
        auto newX = (x + (tileX * World_Constants::CH_SIZE));
        auto newZ = (z + (tileZ * World_Constants::CH_SIZE));

        if (newX < 0 || newZ < 0) return 74;

        auto totalValue   = 0.0;

        //Shorthand
        Data nf = m_noiseFunction;

        for (auto a = 0; a < nf.octaves - 1; a++)      //This loops trough the octaves.
        {
            auto frequency = pow(2.0, a) * 1.5;           //This increases the frequency with every loop of the octave.
            auto amplitude = pow(nf.roughness, a);  //This decreases the amplitude with every loop of the octave.
            totalValue += noise(((double)newX) * frequency / nf.smoother,
                                ((double)newZ) * frequency / nf.smoother)
                                * amplitude;
        }

        return ((totalValue / 2.1) + 1.2) * nf.amplitudeMultiplier;
    }

    void Generator::setNoiseFunction(const Noise::Data& data)
    {
        m_noiseFunction = data;
    }

    uint64_t Generator::getSeed()
    {
        return m_seed;
    }

    void Generator::setSeed(uint64_t newSeed)
    {
        m_seed = newSeed;
    }

}
