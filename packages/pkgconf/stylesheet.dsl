<!-- {{{ Banner                 -->

<!-- =============================================================== -->
<!--                                                                 -->
<!--     stylesheet.sgml                                             -->
<!--                                                                 -->
<!--     Customize the nwalsh modular stylesheets.                   -->
<!--                                                                 -->
<!-- =============================================================== -->
<!-- ####COPYRIGHTBEGIN####                                          -->
<!--                                                                 -->
<!-- =============================================================== -->
<!-- Copyright (C) 2000, 2001 Red Hat, Inc.                                -->
<!--                                                                 -->
<!-- This file is part of the eCos host tools.                       -->
<!--                                                                 -->
<!-- This program is free software; you can redistribute it and/or   -->
<!-- modify it under the terms of the GNU General Public License as  -->
<!-- published by the Free Software Foundation; either version 2 of  -->
<!-- the License, or (at your option) any later version.             -->
<!--                                                                 -->
<!-- This program is distributed in the hope that it will be useful, -->
<!-- but WITHOUT ANY WARRANTY; without even the implied warranty of  -->
<!-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the   -->
<!-- GNU General Public License for more details.                    -->
<!--                                                                 -->
<!-- You should have received a copy of the GNU General Public       -->
<!-- License along with this program; if not, write to the Free      -->
<!-- Software Foundation, Inc.,  59 Temple Place - Suite 330,        -->
<!-- Boston, MA  02111-1307, USA.                                    -->
<!--                                                                 -->
<!-- =============================================================== -->
<!--                                                                 -->      
<!-- ####COPYRIGHTEND####                                            -->
<!-- =============================================================== -->
<!-- #####DESCRIPTIONBEGIN####                                       -->
<!--                                                                 -->
<!-- Author(s):   bartv                                              -->
<!--              Based on cygnus-both.dsl by Mark Galassi           -->
<!-- Contact(s):  bartv                                              -->
<!-- Date:        2000/03/15                                         -->
<!-- Version:     0.01                                               -->
<!--                                                                 -->
<!-- ####DESCRIPTIONEND####                                          -->
<!-- =============================================================== -->

<!-- }}} -->

<!DOCTYPE style-sheet PUBLIC "-//James Clark//DTD DSSSL Style Sheet//EN" [
<!ENTITY % html "IGNORE">
<![%html;[
<!ENTITY % print "IGNORE">
<!ENTITY docbook.dsl PUBLIC "-//Norman Walsh//DOCUMENT DocBook HTML Stylesheet//EN" CDATA dsssl>
]]>
<!ENTITY % print "INCLUDE">
<![%print;[
<!ENTITY docbook.dsl PUBLIC "-//Norman Walsh//DOCUMENT DocBook Print Stylesheet//EN" CDATA dsssl>
]]>
]>

<style-sheet>

<style-specification id="print" use="docbook">
<style-specification-body> 

;; ====================
;; customize the print stylesheet
;; ====================

;; Set the paper parameters as per other eCos documentation
(define %page-width%   7.5in)
(define %page-height%  10.2in)
(define %left-margin%  0.75in)
(define %right-margin% 0.75in)

;; Assume that we are only producing books, a reasonable assumption
;; given the primary author :-)
(define %two-side% #t)

;; Use 12pt
;;(define %visual-acuity% "presbyopic")

;; Do not use graphics in admonitions, our documentation is supposed
;; to look boring :-(
(define %admon-graphics% #f)

;; Justified text please.
(define %default-quadding% 'justify)

;; A separate page for each man page please.
(define %refentry-new-page% #t)

;; The component writer's guide man pages do not describe functions
(define %refentry-functions% #f)

</style-specification-body>
</style-specification>

<!--
;; ====================
;; customize the html stylesheet
;; ====================
-->
<style-specification id="html" use="docbook">
<style-specification-body> 

;; .html files please. 
(define %html-ext% ".html")

;; Boring admonitions
(define %admon-graphics% #f)

(define %shade-verbatim% #t)

;; Use ID attributes as name for component HTML files?
(define %use-id-as-filename% #t)

</style-specification-body>
</style-specification>

<external-specification id="docbook" document="docbook.dsl">

</style-sheet>

