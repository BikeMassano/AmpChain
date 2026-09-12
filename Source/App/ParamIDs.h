namespace ParamIDs
{
    namespace Gate
    {
        inline constexpr auto Threshold = "gate.threshold";
        inline constexpr auto Attack    = "gate.attack";
        inline constexpr auto Release   = "gate.release";
        inline constexpr auto Bypass    = "gate.bypass";
    }

    namespace Compressor
    {
        inline constexpr auto Threshold = "comp.threshold";
        inline constexpr auto Ratio     = "comp.ratio";
        inline constexpr auto Attack    = "comp.attack";
        inline constexpr auto Release   = "comp.release";
        inline constexpr auto Makeup    = "comp.makeup";
        inline constexpr auto Bypass    = "comp.bypass";
    }

    namespace Distortion
    {
        inline constexpr auto Tone      = "dist.tone";
        inline constexpr auto Level     = "dist.level";
        inline constexpr auto Dist      = "dist.dist";
        inline constexpr auto Bypass    = "dist.bypass";
    }

    namespace Amplifier
    {
        inline constexpr auto Bass      = "amp.bass";
        inline constexpr auto Mid       = "amp.mid";
        inline constexpr auto Treble    = "amp.treble";
        inline constexpr auto Level     = "amp.level";
        inline constexpr auto Gain      = "amp.gain";
        inline constexpr auto Presence  = "amp.presence";
        inline constexpr auto Bypass    = "amp.bypass";
    }

    namespace Flanger
    {
        inline constexpr auto Rate      = "flanger.rate";
        inline constexpr auto Range     = "flanger.range";
        inline constexpr auto Feedback  = "flanger.feedback";
        inline constexpr auto Bypass    = "flanger.bypass";
    }

    namespace Cabinet
    {
        inline constexpr auto Bypass    = "cab.bypass";
        inline constexpr auto IRPath    = "cab.IRPath";
    }
}