(compile-options cc-options: "-w -I/data/projects/smartmatter/ipcc"
                 ld-options: "-lrt"
                 force-compile: #t)


(c-declare #<<end-of-c-declare
           
#include <iostream>
#include <csignal>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

#include "../common/ipcvolume.hpp"

template <class T>
class Core
{
    public:

	typedef Lattice<T> LatticeType;
	typedef typename LatticeType::Voxel VoxelType;

	private:

	//! Name of the shared memory space
	const char *_shmName;
	//! Shared memory to allocate
	boost::interprocess::shared_memory_object *_shm_obj;
	//! Process mapping of the memory
	boost::interprocess::mapped_region *_region;
	//! Voxel lattice
	LatticeType *_lattice;
    //! A counter for the total of steps done
    uint _step_counter;

	public:

	//! Constructor
	Core() : _shmName("ipcc_shared_memory")
            , _step_counter(0) {}

	//! Destructor
	virtual ~Core()
    {
        shutdown();
    }

	//! Initialize the core
	int init()
    {
		using namespace std;
		using namespace boost::interprocess;
		try
        {
			shared_memory_object::remove(_shmName);

			_shm_obj = new shared_memory_object(create_only , _shmName, read_write);
			_shm_obj->truncate(sizeof(LatticeType));

			_region = new mapped_region(*_shm_obj, read_write);

			LatticeType *mem = static_cast<LatticeType*>(_region->get_address());
			_lattice = new(mem) LatticeType;
		} catch(interprocess_exception &e)
        {
			shared_memory_object::remove(_shmName);
			cout << e.what() << endl;
			return 1;
		}
		return 0;
	}

	//! Shutdown the core
	int shutdown()
    {
		using namespace std;
		using namespace boost::interprocess;
		try
        {
			shared_memory_object::remove(_shmName);
		} catch(interprocess_exception &e)
        {
			cout << e.what() << endl;
			return 1;
		}
		return 0;
	}

	//! Set a value of the lattice
	void set(int i, int j, int k, VoxelType val)
    {
        _lattice->voxels[i][j][k] = val;
    }

	//! Get a value out of the lattice
	VoxelType get(int i, int j, int k)
    {
        return _lattice->voxels[i][j][k];
    }

    //! Get step counter
    uint getCounter()
    {
        return _step_counter;
    }

    //! Calculate the next iteration of the core CA
    //! Returns 0 if the computation is not over (more steps remaining)
    //! Returns an exit or error code otherwise
    int step()
    {
		using namespace std;
		using namespace boost::interprocess;
        cout << "--> begin step!" << endl;

        {
            scoped_lock<interprocess_mutex> lock(_lattice->header.mutex);
            for(int i=0; i<_lattice->size_x; i++)
                for(int j=0; j<_lattice->size_y; j++)
                    for(int k=0; k<_lattice->size_z; k++) {
                        cout << i << ", " << j << ", " << k << endl;
                    }
            // lock released here
        }

        cout << "--> end step!" << endl;

        _step_counter++;
        if (_step_counter >= 40000000)
        {
            return 99;
        }
        else
        {
            return 0;
        }
    }
};


typedef Core<char> CharCore;
typedef CharCore::LatticeType CharLattice;
typedef CharCore::VoxelType CharVoxel;

end-of-c-declare
)

(c-define-type CharCore "CharCore")
(c-define-type CharLattice "CharLattice")
(c-define-type CharVoxel "CharVoxel")

(define char-core-create
  (c-lambda ()
            (pointer CharCore)
            "
            ___result_voidstar = new CharCore();
            "))

(define char-core-init
  (c-lambda ((pointer CharCore))
            int
            "
            ___result = ___arg1->init();
            "))

(define char-core-shutdown
  (c-lambda ((pointer CharCore))
            int
            "
            ___result = ___arg1->shutdown();
            "))

(define char-core-set
  (c-lambda ((pointer CharCore) int int int CharVoxel)
            void
            "
            ___arg1->set(___arg2,___arg3,___arg4,___arg5);
            "))

(define char-core-get
  (c-lambda ((pointer CharCore) int int int)
            CharVoxel
            "
           ___arg1->get(___arg2,___arg3,___arg4);
           "))

(define char-core-get-counter
  (c-lambda ((pointer CharCore))
            unsigned-int
            "
           ___result = ___arg1->getCounter();
           "))

(define char-core-step
  (c-lambda ((pointer CharCore))
            int
            "
            ___result = ___arg1->step();
            "))


(let ((char-core (char-core-create)))
  (pp (char-core-init char-core))
  (let loop ((status 0))
    (if (= status 0)
        (loop (char-core-step char-core))))
  (pp (char-core-get-counter char-core))
  (pp (char-core-shutdown char-core))
  (display "All right!\n"))


;; (c-declare #<<c-declare-end

;; #include <fstream>

;; ___SCMOBJ delete_ofstream(void *ptr)
;; {
;;  delete (std::ofstream*)ptr;
;;  return ___NO_ERR;
;; }

;; c-declare-end
;; )

;; (c-define-type std::ofstream "std::ofstream")
;; (c-define-type std::ofstream*
;;               (pointer std::ofstream (std::ofstream*)))
;; (c-define-type std::ofstream*/GC
;;               (pointer std::ofstream (std::ofstream*) "delete_ofstream"))

;; (define new-ofstream
;;  (c-lambda (nonnull-char-string) std::ofstream*/GC
;;            "___result_voidstar = new std::ofstream(___arg1);"))

;; (define ofstream-write-string
;;  (c-lambda (std::ofstream* nonnull-char-string) void
;;            "*___arg1 << ___arg2;"))

;; (define ofstream-close
;;  (c-lambda (std::ofstream*) void
;;            "___arg1->close();"))

;; ;; test:

;; (let ((s (new-ofstream "bar")))
;;  (pretty-print s)
;;  (ofstream-write-string s "hello!\n")
;;  (ofstream-close s))
