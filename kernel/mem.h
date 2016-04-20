#ifndef MEM_H
#define MEM_H

#include <inc/memlayout.h>
#include <inc/assert.h>

extern char             bootstacktop[], bootstack[];
extern struct PageInfo  *pages;
extern size_t           npages;
extern pde_t            *kern_pgdir;

/* This macro takes a kernel virtual address -- an address that points above
 * KERNBASE, where the machine's maximum 256MB of physical memory is mapped --
 * and returns the corresponding physical address.  It panics if you pass it a
 * non-kernel virtual address.
 */
#define PADDR(kva) _paddr(__FILE__, __LINE__, kva)

/* This macro takes a physical address and returns the corresponding kernel
 * virtual address.  It panics if you pass an invalid physical address. */
#define KADDR(pa) _kaddr(__FILE__, __LINE__, pa)

enum {
	// For page_alloc, zero the returned physical page.
	ALLOC_ZERO = 1<<0,
};

/* -------------- Prototypes --------------  */

void              mem_init                (void);
void              page_init               (void);
void	            page_free               (struct PageInfo *pp);
int	              page_insert             (pde_t *pgdir, struct PageInfo *pp, void *va, int perm);
void	            page_remove             (pde_t *pgdir, void *va);
void	            ptable_remove           (pde_t *pgdir);
void	            pgdir_remove            (pde_t *pgdir);
void	            page_decref             (struct PageInfo *pp);
struct PageInfo   *page_alloc             (int alloc_flags);
struct PageInfo   *page_lookup            (pde_t *pgdir, void *va, pte_t **pte_store);
pde_t             *setupkvm               (void);
void              setupvm                 (pde_t *pgdir, uint32_t start, uint32_t size);
pte_t             *pgdir_walk             (pde_t *pgdir, const void *va, int create);
void	            tlb_invalidate          (pde_t *pgdir, void *va);
void              mem_init                (void);

int32_t           sys_get_num_free_page   (void);
int32_t           sys_get_num_used_page   (void);


/* -------------- Inline Functions --------------  */

static inline physaddr_t
_paddr(const char *file, int line, void *kva)
{
	if ((uint32_t)kva < KERNBASE)
		_panic(file, line, "PADDR called with invalid kva %08lx", kva);
	return (physaddr_t)kva - KERNBASE;
}

static inline void*
_kaddr(const char *file, int line, physaddr_t pa)
{
	if (PGNUM(pa) >= npages)
		_panic(file, line, "KADDR called with invalid pa %08lx", pa);
	return (void *)(pa + KERNBASE);
}

static inline physaddr_t
page2pa(struct PageInfo *pp)
{
	return (pp - pages) << PGSHIFT;
}

static inline struct PageInfo*
pa2page(physaddr_t pa)
{
	if (PGNUM(pa) >= npages)
		panic("pa2page called with invalid pa");
	return &pages[PGNUM(pa)];
}

static inline void*
page2kva(struct PageInfo *pp)
{
	return KADDR(page2pa(pp));
}

#endif
