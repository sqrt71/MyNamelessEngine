using Scripts.Components;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Scripts.Tests
{
    public class dWall : Actor
    {
        private RigidBodyComponent rigidBody;
        private StaticMeshRenderer staticMesh;

        public dWall()
        {

        }

        protected internal override void Init()
        {
            rigidBody = AddComponent<RigidBodyCubeComponent>("RigidBodyCubeComponent");
            rigidBody.SetType(RigidBodyType.Dynamic);
            rigidBody.SetUsage(RigidBodyUsage.CollisionsAndPhysics);
            rigidBody.SetCollisionShape(CollisionShape.Box);

            staticMesh = AddComponent<StaticMeshRenderer>("StaticMeshRenderer");
            staticMesh.SetMeshPath("../Assets/box.fbx/Cube");
        }
    }
}
