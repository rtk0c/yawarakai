;; => '()
(define (make-counter n)
  (lambda ()
    (set! n (+ n 1))
    n))

;; => '()
(define c1 (make-counter 0))
;; => 1
(c1)
;; => 2
(c1)

;; => '()
(define c2 (make-counter 0))
;; => 1
(c2)
