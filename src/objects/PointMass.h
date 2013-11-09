#ifndef POINTMASS_H
#define POINTMASS_H

class PointMass
{
public:
	PointMass();
	PointMass(PointMass *pPtMass)
	{
		m_Mass = pPtMass->m_Mass;
		m_Position = pPtMass->m_Position;
		m_Tag = pPtMass->m_Tag;
	}

	PointMass(double const &m, CVector const &p, QString const &tag)
	{
		m_Mass = m;
		m_Position = p;
		m_Tag = tag;
	}

	double m_Mass;          /**< the value of the point mass, in kg */
	CVector m_Position;      /**< the absolute position of the point mass */
	QString m_Tag;           /**< the description of the point mass */

	double mass()      {return m_Mass;}
	CVector position() {return m_Position;}
	QString tag()      {return m_Tag;}
};

#endif // POINTMASS_H
