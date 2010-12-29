(compile-options cc-options: "-w -I/data/projects/smartmatter/ipcc"
                 ld-options: "-lrt"
                 force-compile: #t)


(c-declare #<<end-of-c-declare
           
#include "core.hpp"

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
