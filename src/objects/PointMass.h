#ifndef POINTMASS_H
#define POINTMASS_H

class PointMass
{
public:
	/** The public constructor */
	PointMass();

	/** Overloaded public constructor */
	PointMass(PointMass *pPtMass)
	{
		m_Mass = pPtMass->m_Mass;
		m_Position = pPtMass->m_Position;
		m_Tag = pPtMass->m_Tag;
	}

	/** Overloaded public constructor */
	PointMass(double const &m, CVector const &p, QString const &tag)
	{
		m_Mass = m;
		m_Position = p;
		m_Tag = tag;
	}

	double m_Mass;          /**< the value of the point mass, in kg */
	CVector m_Position;      /**< the absolute position of the point mass */
	QString m_Tag;           /**< the description of the point mass */

	/** Returns the the value of the mass */
	double mass()      {return m_Mass;}
	
	/** Returns the the position of the mass */
	CVector position() {return m_Position;}
	
	/** Returns the the tag of the mass */
	QString tag()      {return m_Tag;}
};

#endif // POINTMASS_H
