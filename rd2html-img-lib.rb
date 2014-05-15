require "rd/rd2html-lib"

module RD
  class RD2HTMLVisitor

    # RDVisitor#apply_to_Include 
    def apply_to_Include(content)
      if /^(.+\.(?:png|gif|jpg))(?::(.+))?$/ === content.filename
	m =  %Q[<p><img src="#{$1}" ]
	alt = $2 || $1
	m << %Q[alt="#{alt}"] 
	m << %Q[/></p>\n]
	m
      else
	super
      end
    end

  end
end
