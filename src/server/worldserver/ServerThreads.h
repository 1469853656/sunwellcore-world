#ifndef __WORLDRUNNABLE_H
#define __WORLDRUNNABLE_H

namespace Sunwell
{
    void WorldThread();
    void AuctionListingThread();
    void FreezeDetectorThread(uint32 _delayTime);
    void CommandLineThread();
}
#endif
