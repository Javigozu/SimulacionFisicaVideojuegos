#include "Projectile.h"
#include "Vector3D.h"

Projectile::Projectile(Vector3D p, Vector3D vDir, double spR, double spS, double mR, double d, double gR)
	: Particle(p, vDir.normalize()* spS, { 0.0, 0.0, 0.0 }, d, massSimulated(mR, spR, spS), gravitySimulated(gR, spR, spS)), speed(spS)
{
	setAcc({ 0.0, - gravity, 0.0 });
}
double Projectile::massSimulated(double mR, double spR, double spS) {
	return (mR * spR * spR) / (spS * spS);
}
double Projectile::gravitySimulated(double gR, double spR, double spS) {
	return (gR * spS * spS) / (spR * spR);
}
