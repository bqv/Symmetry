#lang racket

(require racket/tcp)

(define-values (in out) (tcp-connect "irc.subluminal.net" 6667))
;(define-values (in out) (tcp-connect "192.168.1.65" 4444))

(define-syntax-rule (debug str ...)
                    (let ()
                      (displayln (string-append "-> " str ...))
                      (displayln (string-append str ...) out)))

(define connected #t)
(define welcome #f)
(define autojoin "#programming")

(define-syntax-rule (put str num)
  (let ()
   (namespace-set-variable-value! (string->symbol (substring str 1)) num)
   num))
(define-syntax-rule (get str)
  (with-handlers ([exn:fail:contract:variable?
		   (lambda (e) (put str 0))])
		 (namespace-variable-value (string->symbol (substring str 1)))))

(debug "USER symmetry * * *")
(debug "NICK it")
(debug "PASS him/awfulnot:him")
(flush-output out)

(define (weigh-line str)
  (for/sum ([i (map (lambda (c)
                      (if (or (equal? c #\() (equal? c #\[)) +1 (if (or (equal? c #\)) (equal? c #\])) -1 0)))
                    (string->list str))]) i))

(let loop ()
  (when connected
    (let ([line (read-line in)])
      (when (eof-object? line)
        (displayln "Server hungup...")
        (set! connected #f))
      (displayln line)

      (when (not (equal? line ""))
        (let ([line (string-split line)])
          (when (equal? (car line) "PING")
            (debug "PONG " (cadr line)))
          (when (not welcome)
            (when (equal? (cadr line) "001")
              (set! welcome #t)
              (debug "JOIN " autojoin)))

	  (when (equal? (cadr line) "JOIN")
		(displayln (get (caddr line))))
          (when (equal? (cadr line) "PRIVMSG")
            (let ([text (string-downcase (substring (string-join (cdddr line)) 1))])
              (displayln (string-append "<- " text ))
              (if (regexp-match #rx" *@symmetry *" text)
                (debug "PRIVMSG " (caddr line) " :" (number->string (get (caddr line))))
                (put (caddr line) (+ (weigh-line (substring (cadddr line) 1)) (get (caddr line)))))
              (when (regexp-match #rx" *@rebalance *" text)
                (put (caddr line) 0))))
          (when (equal? (cadr line) "INVITE")
            (let ([text (string-downcase (substring (string-join (cdddr line)) 1))])
              (displayln (string-append "[INV] " text ))
              (debug "JOIN " text))))

        (flush-output out)))
    (loop)))
