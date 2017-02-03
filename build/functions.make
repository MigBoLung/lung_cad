# $(call make-depend,source-file,object-file,depend-file)

define make-depend

  $(CC)  -MM            \
         -MF $3         \
         -MP            \
         -MT $2         \
         $(INCLUDES)    \
         $(CFLAGS)      \
         $(CPPFLAGS)    \
         $1

endef