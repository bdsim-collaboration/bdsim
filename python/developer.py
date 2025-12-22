
class developer :
    classes = ['Aperture', 'Atom', 'Beam', 'BLMPlacement', 'CavityModel', 'CoolingChannel', 'Crystal',
               'Element', 'Field', 'Material', 'Modulator', 'NewColour', 'Options', 'PhysicsBiasing',
               'Placement', 'Query', 'Region', 'SamplerPlacement', 'ScorerMesh', 'Scorer', 'Tunnel']

    @staticmethod
    def check_published_attributes(obj) :
        iNotFound = 0
        for n in obj.AllNames():
            try :
                getattr(obj,n)
            except AttributeError:
                print("Method not found : "+n)
                iNotFound += 1

        return iNotFound

    @staticmethod
    def check_all_classes():
        import bdsim

        iNotFound = 0
        for c in developer.classes :
            cls = getattr(bdsim, c)
            inst = cls()
            print("Class name : ", c)
            iNotFound += developer.check_published_attributes(inst)

        print("Number methods not found :", iNotFound)