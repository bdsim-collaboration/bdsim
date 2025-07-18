import RF_Track
import bdsim

class BDSIMElement(RF_Track.UserElement) :
    def __init__(self, e):
        print("python> BDSIMElement")
        super().__init__(e.l)

        self.bds_link = bdsim.BDSLinkTrackerInterface.GetInstance()
        e_idx = self.bds_link.GetBDSIMLink().AddLinkElement(e)

        self.index = e_idx

        self.bds_link.GetBDSIMLink().SelectLinkElement(e_idx)


    def track(self, bunch6d):
        print("python track> ",bunch6d)
        self.bds_link.TrackRFTrack(bunch6d)