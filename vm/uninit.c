/* uninit.c: Implementation of uninitialized page.
 *
 * All of the pages are born as uninit page. When the first page fault occurs,
 * the handler chain calls uninit_initialize (page->operations.swap_in).
 * The uninit_initialize function transmutes the page into the specific page
 * object (anon, file, page_cache), by initializing the page object,and calls
 * initialization callback that passed from vm_alloc_page_with_initializer
 * function.
 * */

#include "vm/vm.h"
#include "vm/uninit.h"

static bool uninit_initialize(struct page *page, void *kva);
static void uninit_destroy(struct page *page);

/* DO NOT MODIFY this struct */
static const struct page_operations uninit_ops = {
		.swap_in = uninit_initialize,
		.swap_out = NULL,
		.destroy = uninit_destroy,
		.type = VM_UNINIT,
};

/* DO NOT MODIFY this function */
// 인자로 받은 VM 초기화 함수, 페이지 타입, 페이지 초기화 함수를 PAGE 멤버에 채워준다.
void uninit_new(struct page *page, void *va, vm_initializer *init,
								enum vm_type type, void *aux,
								bool (*initializer)(struct page *, enum vm_type, void *)) {
	ASSERT(page != NULL);
	*page = (struct page){
			.operations = &uninit_ops,
			.va = va,
			.frame = NULL, /* no frame for now */
			.uninit = (struct uninit_page){
					.init = init,
					.type = type,
					.aux = aux,
					.page_initializer = initializer,
			}};
}

/* Initalize the page on first fault */
static bool
uninit_initialize(struct page *page, void *kva)
{
	struct uninit_page *uninit = &page->uninit;

	/* Fetch first, page_initialize may overwrite the values */
	vm_initializer *init = uninit->init;
	void *aux = uninit->aux;

	/* TODO: You may need to fix this function. */
	// 해당 페이지의 타입에 맞도록 페이지를 초기화한다.
	// 만약 해당 페이지의 segment가 load되지 않은 상태면 lazy_load 해준다. => init이 lazy_load_segment일때
	return uninit->page_initializer(page, uninit->type, kva) && (init ? init(page, aux) : true);
}

/* Free the resources hold by uninit_page. Although most of pages are transmuted
 * to other page objects, it is possible to have uninit pages when the process
 * exit, which are never referenced during the execution.
 * PAGE will be freed by the caller. */
static void
uninit_destroy(struct page *page)
{
	struct uninit_page *uninit UNUSED = &page->uninit;
	/* TODO: Fill this function.
	 * TODO: If you don't have anything to do, just return. */
		struct file_info *file_info;
	if(VM_TYPE(uninit->type) == VM_ANON){
		file_info = (struct file_info*)uninit->aux;
		free(file_info);
	}

}
