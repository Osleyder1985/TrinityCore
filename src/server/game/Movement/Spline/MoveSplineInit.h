/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TRINITYSERVER_MOVESPLINEINIT_H
#define TRINITYSERVER_MOVESPLINEINIT_H

#include "MoveSplineInitArgs.h"
#include <variant>

class ObjectGuid;
class Unit;

struct Position;
enum class AnimTier : uint8;

namespace Movement
{
    /*  Initializes and launches spline movement
     */
    class TC_GAME_API MoveSplineInit
    {
    public:

        explicit MoveSplineInit(Unit* m);

        ~MoveSplineInit();

        MoveSplineInit(MoveSplineInit const&) = delete;
        MoveSplineInit& operator=(MoveSplineInit const&) = delete;
        MoveSplineInit(MoveSplineInit&& init) = delete;
        MoveSplineInit& operator=(MoveSplineInit&&) = delete;

        /*  Final pass of initialization that launches spline movement.
         */
        int32 Launch();

        /*  Final pass of initialization that stops movement.
         */
        void Stop();

        /* Adds movement by parabolic trajectory
         * @param amplitude  - the maximum height of parabola, value could be negative and positive
         * @param start_time - delay between movement starting time and beginning to move by parabolic trajectory
         * can't be combined with final animation
         */
        void SetParabolic(float amplitude, float start_time);
        /* Adds movement by parabolic trajectory
         * @param vertical_acceleration - vertical acceleration
         * @param start_time - delay between movement starting time and beginning to move by parabolic trajectory
         * can't be combined with final animation
         */
        void SetParabolicVerticalAcceleration(float vertical_acceleration, float start_time);
        /* Plays animation after movement done
         * can't be combined with parabolic movement
         */
        void SetAnimation(AnimTier anim, uint32 tierTransitionId = 0, Milliseconds transitionStartTime = 0ms);

        /* Adds final facing animation
         * sets unit's facing to specified point/angle after all path done
         * you can have only one final facing: previous will be overriden
         */
        void SetFacing(float angle);
        void SetFacing(Vector3 const& spot);
        void SetFacing(float x, float y, float z);
        void SetFacing(Unit const* target);

        /* Initializes movement by path
         * @param path - array of points, shouldn't be empty
         * @param pointId - Id of fisrt point of the path. Example: when third path point will be done it will notify that pointId + 3 done
         */
        void MovebyPath(PointsArray const& path, int32 pointId = 0);

        /* Initializes simple A to B motion, A is current unit's position, B is destination
         */
        void MoveTo(Vector3 const& destination, bool generatePath = true, bool forceDestination = false);
        void MoveTo(float x, float y, float z, bool generatePath = true, bool forceDestination = false);

        /* Sets Id of fisrt point of the path. When N-th path point will be done ILisener will notify that pointId + N done
         * Needed for waypoint movement where path splitten into parts
         */
        void SetFirstPointId(int32 pointId) { args.path_Idx_offset = pointId; }

        /* Enables CatmullRom spline interpolation mode(makes path smooth)
         * if not enabled linear spline mode will be choosen. Disabled by default
         */
        void SetSmooth();

        /* Waypoints in packets will be sent without compression
         */
        void SetUncompressed();

        /* Enables flying animation. Disabled by default
         */
        void SetFly();

        /* Enables walk mode. Disabled by default
         */
        void SetWalk(bool enable);

        /* Makes movement cyclic. Disabled by default
         */
        void SetCyclic();

        /* Enables falling mode. Disabled by default
         */
        void SetFall();

        /* Enters transport. Disabled by default
         */
        void SetTransportEnter();

        /* Exits transport. Disabled by default
         */
        void SetTransportExit();

        /* Inverses unit model orientation. Disabled by default
         */
        void SetBackward();

        /* Fixes unit's model rotation (plays knockback animation). Disabled by default
         */
        void SetOrientationFixed(bool enable);

        /* Fixes unit's model rotation (plays jump animation). Disabled by default
         */
        void SetJumpOrientationFixed(bool enable);

        /* Enables avoiding minor obstacles clientside (might cause visual position on client to not be accurate with the serverside one). Disabled by default
         */
        void SetSteering();

        /* Enables no-speed limit
         * if not set, the speed will be limited by certain flags to 50.0f, and otherwise 28.0f
         */
        void SetUnlimitedSpeed();

        /* Sets the velocity (in case you want to have custom movement velocity)
         * if no set, speed will be selected based on unit's speeds and current movement mode
         * Has no effect if falling mode enabled
         * velocity shouldn't be negative
         */
        void SetVelocity(float velocity);

        void SetSpellEffectExtraData(SpellEffectExtraData const& spellEffectExtraData);

        void SetTurning(float startFacing, float totalTurnRads, float radsPerSec);

        PointsArray& Path() { return args.path; }

        /* Disables transport coordinate transformations for cases where raw offsets are available
        */
        void DisableTransportPathTransformations();
    protected:

        MoveSplineInitArgs args;
        Unit*  unit;
    };

    inline void MoveSplineInit::SetFly() { args.flags.Flying = true; }
    inline void MoveSplineInit::SetWalk(bool enable) { args.walk = enable; }
    inline void MoveSplineInit::SetSmooth() { args.flags.Catmullrom = true; }
    inline void MoveSplineInit::SetUncompressed() { args.flags.UncompressedPath = true; }
    inline void MoveSplineInit::SetCyclic() { args.flags.Cyclic = true; }
    inline void MoveSplineInit::SetVelocity(float vel) { args.velocity = vel; args.HasVelocity = true; }
    inline void MoveSplineInit::SetBackward() { args.flags.Backward = true; }
    inline void MoveSplineInit::SetTransportEnter() { args.flags.TransportEnter = true; }
    inline void MoveSplineInit::SetTransportExit() { args.flags.TransportExit = true; }
    inline void MoveSplineInit::SetOrientationFixed(bool enable) { args.flags.OrientationFixed = enable; }
    inline void MoveSplineInit::SetJumpOrientationFixed(bool enable) { args.flags.JumpOrientationFixed = enable; }
    inline void MoveSplineInit::SetSteering() { args.flags.Steering = true; }
    inline void MoveSplineInit::SetUnlimitedSpeed() { args.flags.UnlimitedSpeed = true; }

    inline void MoveSplineInit::SetParabolic(float amplitude, float start_time)
    {
        args.effect_start_time_percent = start_time;
        args.parabolic_amplitude = amplitude;
        args.vertical_acceleration = 0.0f;
        args.flags.Parabolic = true;
    }

    inline void MoveSplineInit::SetParabolicVerticalAcceleration(float vertical_acceleration, float start_time)
    {
        args.effect_start_time_percent = start_time;
        args.parabolic_amplitude = 0.0f;
        args.vertical_acceleration = vertical_acceleration;
        args.flags.Parabolic = true;
    }

    inline void MoveSplineInit::SetAnimation(AnimTier anim, uint32 tierTransitionId /*= 0*/, Milliseconds transitionStartTime /*= 0ms*/)
    {
        args.effect_start_time_percent = 0.f;
        args.effect_start_time = transitionStartTime;
        args.animTier.emplace();
        args.animTier->TierTransitionId = tierTransitionId;
        args.animTier->AnimTier = anim;
        if (!tierTransitionId)
            args.flags.Animation = true;
    }

    inline void MoveSplineInit::DisableTransportPathTransformations() { args.TransformForTransport = false; }

    inline void MoveSplineInit::SetSpellEffectExtraData(SpellEffectExtraData const& spellEffectExtraData)
    {
        args.spellEffectExtra = spellEffectExtraData;
    }

    inline void MoveSplineInit::SetTurning(float startFacing, float totalTurnRads, float radsPerSec)
    {
        args.flags.Turning = true;

        TurnData& turn = args.turnData.emplace();
        turn.StartFacing = startFacing;
        turn.TotalTurnRads = totalTurnRads;
        turn.RadsPerSec = radsPerSec;
    }

    struct MoveSplineInitFacingVisitor
    {
        explicit MoveSplineInitFacingVisitor(MoveSplineInit& init) : _init(init) { }

        void operator()(std::monostate) const { }
        void operator()(Position const& point) const;
        void operator()(Unit const* target) const { _init.SetFacing(target); }
        void operator()(float angle) const { _init.SetFacing(angle); }

        MoveSplineInit& _init;
    };
}

#endif // TRINITYSERVER_MOVESPLINEINIT_H
