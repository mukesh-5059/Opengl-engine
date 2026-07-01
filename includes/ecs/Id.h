#pragma once
#include <cstdint>

namespace Id {
    using idType = uint32_t;
    using EntityId = idType;
    using ComponentIndex = idType;

    constexpr ComponentIndex invalidIndex = -1;

    constexpr idType generationBits = 10;
    constexpr idType generationMask = (1 << generationBits) -1;
    constexpr idType indexBits = (sizeof(idType) * 8 - generationBits);
    constexpr idType indexMask = (1 << indexBits) - 1;
    constexpr idType invalidId = -1;

    inline idType indexOf(idType i){
        return i & indexMask;
    }

    inline idType generationOf(idType i){
        return (i >> indexBits) & generationMask;
    }

    inline idType generateId(idType generation, idType index){
        return (generation << indexBits) | index;
    }
}
