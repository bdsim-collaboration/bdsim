import ocelot
class BDSIMElement(ocelot.cpbd.elements.optic_element.Element) :
    def __init__(self, l, eid=None):
        self.l = l
        super().__init__(eid=eid, has_edge=False)

        self.tms = []

    def set_tm(self, tm_class_type):
        pass