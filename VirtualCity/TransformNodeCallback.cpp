#include "TransformNodeCallback.h"
#include <assert.h>


#define LENGTH 15.0
#define  SIZE 5.0

CTransformNodeCallback::CTransformNodeCallback(osg::AnimationPath* ap, std::vector<crossingstruct>* crossingArray,std::vector<stindex> indexArray,unsigned int loc):osg::AnimationPathCallback(ap),m_crossingArray(crossingArray),m_indexArray(indexArray),m_count(0)
{ 
	 osg::AnimationPath::ControlPoint cp;
	 osg::AnimationPath *aa = getAnimationPath();
	 aa ->getInterpolatedControlPoint(0.3,cp);
	 m_bs._center = cp.getPosition();
	 m_bs._radius = 1.0;	
	 m_indexArray = indexArray;
	 m_length = LENGTH + loc * 1.0;
	 m_dirction.set(0.0,1.0,0.0);
}



void CTransformNodeCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{


	if (_animationPath.valid() && 
		nv->getVisitorType()==osg::NodeVisitor::UPDATE_VISITOR && 
		nv->getFrameStamp())
	{
		double time = nv->getFrameStamp()->getSimulationTime();
		_latestTime = time;

		if (!_pause)
		{
			// Only update _firstTime the first time, when its value is still DBL_MAX
			if (_firstTime==DBL_MAX) _firstTime = time;
			update(*node);
		}
	}



	osg::Vec3 tmp1 = node->getBound()._center - m_bs._center;
	//m_size = 2 * node->getBound()._radius;

	osg::Vec3 tmp2(0.0,0.0,0.0);
	if(tmp1 != tmp2)
		m_dirction = tmp1;

// 	double d = tmp1.length();
// 	if( d > 1.0)
// 		m_dirction = tmp1;

	m_dirction.normalize();
	m_bs = node->getBound();
	

	if ( isStopByCar( node->getParent(0)))			//�ж��Ƿ���������������ײ
	{
		setPause( true);
 		m_hitVisitor.setIsCrush( false);
 	}
	else
		setPause( isStopBySignal())	;				//�ж�С����·���뽻ͨ�Ƶ����
		



}


bool CTransformNodeCallback::isStopByCar(osg::Node* node)
{
	osg::Vec3 begin = m_bs._center;
	osg::Vec3 end  = m_bs._center + m_dirction * SIZE;

	m_hitVisitor.setCenter(end);
	node->accept(m_hitVisitor);

	
	return m_hitVisitor.getIsCrush();

}

bool CTransformNodeCallback::isStopBySignal()
{
	std::vector<crossingstruct>* crossingArray = m_crossingArray;
	std::vector<stindex> indexarry = m_indexArray;
	if(m_indexArray.size() == 0)
		return false;
	if( m_count == m_indexArray.size())
		m_count = 0;
	int i = m_indexArray[m_count].crossing;
	int j = m_indexArray[m_count].direction;
	osg::Vec3 vec = (*m_crossingArray)[i].point - m_bs.center();
	double length = vec.length();
	if (length < m_length)
	{
		std::vector<roadstruct> roadArray = (*m_crossingArray)[i].roadArray;
		if ( roadArray[j].color == RED)
		{
	
			return true;
		}
		else
			if (roadArray[j].color == GREEN)
			{
				m_count++;
				return false;
			}


	}

	return false;

}