#include "player.h"

Player::Player()
{
    setRadius(Vector3(0.25, 0.5, 0.25));
    setStartPoint(Vector3(0, 1.5, 0));

    vup = -2;
    height = 0;
}

bool Player::calculateHitPoint(const OBJ &obj)
{
    float tmin = INFINITY, t1 = INFINITY, t2 = INFINITY;

    Vector3 b, a, spnormal, eplinormal, c, d, v, potential, contact;
    float ae, be, ce; //efficient

    for(int i =0; i< obj.triangles.size(); i++)
    {
        Triangle t = obj.triangles.at(i);
        TriangleInstance el_tri(obj.vertices[t.a.vertex], obj.vertices[t.b.vertex], obj.vertices[t.c.vertex]);
        TriangleInstance sp_tri = el_tri.scale(1/radius);

        b = sp_end;
        a = sp_start;

        //triangle interior
        if((end - start).dot(el_tri.normal)>0 )
        {
            spnormal = -sp_tri.normal;
            eplinormal = -el_tri.normal;
        }
        else
        {
            spnormal = sp_tri.normal;
            eplinormal = el_tri.normal;
        }

        t1  = - (spnormal.dot(a-spnormal-sp_tri.vertices[0]))/(spnormal.dot(b-a));
        potential = a-spnormal + (b-a)*t1;
        if(t1 >=0 && t1 <= 1 && t1 < tmin && checkInsideTriangle(potential, sp_tri))
        {
            tmin = t1;
            cur_normal = eplinormal;
        }

        //triangle vertices and edges
        for(int j= 0; j< 3; j++)
        {
            //vertices
            v = sp_tri.vertices[j];
            ae = (b-a).dot(b-a);
            be = -2*(b-a).dot(v-a);
            ce = (v-a).dot(v-a) - 1;
            getEquationRoot(ae,be,ce,t1,t2);
            if(t1 >= 0 && t1 <= 1)
            {
                if(tmin > t1)
                {
                    tmin = t1;
                    spnormal = potential-v;
                    eplinormal = (spnormal/radius/radius).unit();
                    cur_normal = eplinormal; //contact normal
                }
            }
            if(t2 >= 0 && t2 <=1 )
            {
                if(tmin > t2)
                {
                    tmin = t2;
                    spnormal = potential-v;
                    eplinormal = (spnormal/radius/radius).unit();
                    cur_normal = eplinormal; //contact normal
                }
            }

            //edge
            c = v;
            d = sp_tri.vertices[(j+1)%3];
            ae = ((b-a).cross(d-c)).lengthSquared();
            be = 2*((b-a).cross(d-c)).dot((a-c).cross(d-c));
            ce = ((a-c).cross(d-c)).lengthSquared() - (d-c).lengthSquared();
            getEquationRoot(ae,be,ce,t1,t2);
            if(t1 >= 0 && t1 <= 1)//within range
            {
                potential = a + t1*(b-a);
                if(checkOnEdge(potential,contact,c,d))// in sphere world
                {
                    if(tmin > t1)
                    {
                        tmin = t1;
                        spnormal = potential-contact;
                        eplinormal = (spnormal/radius/radius).unit();
                        cur_normal = eplinormal; //contact normal
                    }
                }

            }
            if(t2 >= 0 && t2 <= 1)
            {
                potential = a + t2*(b-a);
                if(checkOnEdge(potential,contact,c,d))
                {
                    if(tmin > t2)
                    {
                        tmin = t2;
                        spnormal = potential-contact;
                        eplinormal = (spnormal/radius/radius).unit();
                        cur_normal = eplinormal; //contact normal
                    }
                }
            }

        }

    }

    if(tmin<INFINITY)
    {
        collision = start + tmin*(end - start);
        return true;
    }
    else
        return false;
}

bool Player::checkInsideTriangle(Vector3 p, const TriangleInstance &abc)
{
    TriangleInstance pab(p, abc.vertices[0], abc.vertices[1]);
    TriangleInstance pbc(p, abc.vertices[1], abc.vertices[2]);
    TriangleInstance pca(p, abc.vertices[2], abc.vertices[0]);

    if(pab.normal.dot(abc.normal)>0 && pbc.normal.dot(abc.normal)>0 && pca.normal.dot(abc.normal)>0)
        return true;
    else
        return false;
}

bool Player::checkOnEdge(Vector3 p, Vector3 &pc, Vector3 c, Vector3 d)
{
    float maxlimit = (d-c).dot(d-c);
    float test = (p-c).dot(d-c);
    if(test > 0 && test <= maxlimit) // calculate the contact point
    {
        pc = (p-c).dot(d-c)/((d-c).lengthSquared())*(d-c) + c;
        return true;
    }
    else
        return false;
}

void Player::getEquationRoot(float a, float b, float c, float &t1, float &t2)
{
    float delta;
    delta = b*b - 4*a*c;
    if(delta >= 0)
    {
        t1 = (-b + sqrt(delta))/2.0/a;
        t2 = (-b - sqrt(delta))/2.0/a;
    }
    else
        t1 = t2 = INFINITY;
}

void Player::render()
{
    glPushMatrix();
    glColor3f(0, 0.5, 0.7);
    glTranslatef(start.x, start.y, start.z);
    glScalef(radius.x, radius.y, radius.z);
    gluSphere(gluNewQuadric(), 1, 32, 16);
    glPopMatrix();
}

void Player::setDirection(Camera &c)
{
    forward = c.forward;
    right = c.right;
}

void Player::moveRight(Scene &m_s)
{
    eplback = EPSILON*(right);

    setEndPoint(start + right * 0.3);
//    if(calculateHitPoint(m_s.m_obj))
//        setStartPoint(collision-EPSILON*right);
//    else
//        setStartPoint(end);
    iterativeCollisionDetect(m_s.m_obj);

}

void Player::moveLeft(Scene &m_s)
{
    eplback = EPSILON*(-right);
    setEndPoint(start - right * 0.3);
//    if(calculateHitPoint(m_s.m_obj))
//        setStartPoint(collision-EPSILON*(-right));
//    else
//        setStartPoint(end);

    iterativeCollisionDetect(m_s.m_obj);

}

void Player::moveForward(Scene &m_s)
{
    eplback = EPSILON*(forward);

    setEndPoint(start + forward * 0.3);
//    if(calculateHitPoint(m_s.m_obj))
//        setStartPoint(collision-EPSILON*(forward));
//    else
//        setStartPoint(end);
    iterativeCollisionDetect(m_s.m_obj);

}

void Player::moveBack(Scene &m_s)
{
    eplback = EPSILON*(-forward);

    setEndPoint(start - forward * 0.3);
//    if(calculateHitPoint(m_s.m_obj))
//        setStartPoint(collision-EPSILON*(-forward));
//    else
//        setStartPoint(end);
    iterativeCollisionDetect(m_s.m_obj);
}

void Player::moveUp(Scene &m_s, bool &injump, float t)
{
    height += vup * t - 0.5 * 10 * t * t; //the supposed position y right now
    vup += -10 * t;
    Vector3 perpen(0,1,0);

    setEndPoint(Vector3(start.x, height, start.z));
    if(calculateHitPoint(m_s.m_obj))
    {
        vup = -2;
        if(height >= start.y)
            setStartPoint(collision - EPSILON*perpen);
        else
        {
            setStartPoint(collision + EPSILON*perpen);
            injump = false;
        }
        std::cout<<"collision";
    }
    else
    {
        setStartPoint(end);
        std::cout<<"no collision";
    }

    std::cout<<"start.y: "<<start.y<<endl;
}

void Player::iterativeCollisionDetect(OBJ &obj)
{
    //start and end point alreday set up
    int i=0;
    Vector3 perpend, horizon;
    while(i++ < 3)
    {
        if(calculateHitPoint(obj))
        {
            setStartPoint(collision - eplback);
//            cur_normal = -cur_normal;
            perpend = (end-start).dot(cur_normal)*cur_normal;
            horizon = (end-start) - perpend;
            setEndPoint(start + horizon);
            eplback = horizon.unit() * EPSILON;
        }
        else
        {
            setStartPoint(end);
            break;
        }
    }
}



