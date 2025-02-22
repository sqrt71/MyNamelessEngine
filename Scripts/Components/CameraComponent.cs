﻿using System.Runtime.CompilerServices;
using System;
using Scripts.Engine;
using SharpDX;

namespace Scripts.Components
{
    public sealed class CameraComponent : Component
    {
        public CameraComponent(Actor owner, bool internalCreate) : base(owner)
        {
            if (internalCreate)
            {
                this.CppInstance = InternalCreateComponent(owner.CppInstance, (int) ComponentType.CameraComponentType);
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetCameraTransformInternal(IntPtr handle, Vector3 position, Quaternion rotationQuaternion);
        

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float GetMouseDeltaXInternal(IntPtr handle);
        

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float GetMouseDeltaYInternal(IntPtr handle);

        public void SetCameraTransform(Vector3 position, Quaternion rotationQuaternion)
        {
            SetCameraTransformInternal(CppInstance, position, rotationQuaternion);
        }

        public float GetMouseDeltaX()
        {
            return GetMouseDeltaXInternal(CppInstance);
        }

        public float GetMouseDeltaY()
        {
            return GetMouseDeltaYInternal(CppInstance);
        }

    }
}
