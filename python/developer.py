
class developer :
    @staticmethod
    def check_published_attributes(obj) :
        for n in obj.AllNames():
            try :
                getattr(obj,n)
            except AttributeError:
                print(n+" not found")

    @staticmethod
    def check_all_classes():
        classes = []

        for c in classes() :
            pass