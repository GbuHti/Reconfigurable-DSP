#ifndef RISCV_ISA_ELF_LOADER_H
#define RISCV_ISA_ELF_LOADER_H


#include <boost/iostreams/device/mapped_file.hpp>

#include <cstdint>
#include <vector>


// see: http://wiki.osdev.org/ELF_Tutorial
// for ELF definitions

typedef uint16_t Elf32_Half;	// Unsigned half int
typedef uint32_t Elf32_Off;	    // Unsigned offset
typedef uint32_t Elf32_Addr;	// Unsigned address
typedef uint32_t Elf32_Word;	// Unsigned int
typedef int32_t  Elf32_Sword;	// Signed int

#define ELF_NIDENT 16

typedef struct {
    uint8_t		e_ident[ELF_NIDENT];
    Elf32_Half	e_type;
    Elf32_Half	e_machine;
    Elf32_Word	e_version;
    Elf32_Addr	e_entry;
    Elf32_Off	e_phoff;
    Elf32_Off	e_shoff;
    Elf32_Word	e_flags;
    Elf32_Half	e_ehsize;
    Elf32_Half	e_phentsize;
    Elf32_Half	e_phnum;
    Elf32_Half	e_shentsize;
    Elf32_Half	e_shnum;
    Elf32_Half	e_shstrndx;
} Elf32_Ehdr;


typedef struct {
    Elf32_Word	p_type;
    Elf32_Off	p_offset;
    Elf32_Addr	p_vaddr;
    Elf32_Addr	p_paddr;
    Elf32_Word	p_filesz;
    Elf32_Word	p_memsz;
    Elf32_Word	p_flags;
    Elf32_Word	p_align;
} Elf32_Phdr;


typedef struct {
    Elf32_Word	sh_name;
    Elf32_Word	sh_type;
    Elf32_Word	sh_flags;
    Elf32_Addr	sh_addr;
    Elf32_Off	sh_offset;
    Elf32_Word	sh_size;
    Elf32_Word	sh_link;
    Elf32_Word	sh_info;
    Elf32_Word	sh_addralign;
    Elf32_Word	sh_entsize;
} Elf32_Shdr;


typedef struct {
    Elf32_Word st_name;
    Elf32_Addr st_value;
    Elf32_Word st_size;
    unsigned char st_info;
    unsigned char st_other;
    Elf32_Half st_shndx;
} Elf32_Sym;


enum Elf32_PhdrType {
    PT_NULL     = 0,
    PT_LOAD     = 1,
    PT_DYNAMIC  = 2,
    PT_INTERP   = 3,
    PT_NOTE     = 4,
    PT_SHLIB    = 5,
    PT_PHDR     = 6
};


struct ELFLoader {
    const char *filename;
    boost::iostreams::mapped_file_source elf;
    const Elf32_Ehdr *hdr;

    ELFLoader(const char *filename)
        : filename(filename), elf(filename) {
        assert (elf.is_open() && "file not open");

        hdr = reinterpret_cast<const Elf32_Ehdr *>(elf.data());
    }


    std::vector<const Elf32_Phdr *> get_load_sections() {

        std::vector<const Elf32_Phdr *> sections;

        for (int i=0; i<hdr->e_phnum; ++i) {
            const Elf32_Phdr *p = reinterpret_cast<const Elf32_Phdr *>(
                    elf.data() + hdr->e_phoff + hdr->e_phentsize * i);

            if (p->p_type != PT_LOAD)
                continue;

            sections.push_back(p);
        }

        return sections;
    }


    void load_executable_image(uint8_t *dst, uint32_t size, uint32_t offset) {
        for (auto p : get_load_sections()) {

            assert ((p->p_vaddr+p->p_memsz >= offset) && (p->p_vaddr+p->p_memsz < offset+size));

            //NOTE: if memsz is larger than filesz, the additional bytes are zero initialized (auto. done for memory)
            memcpy(dst+p->p_vaddr-offset, elf.data()+p->p_offset, p->p_filesz);
        }
    }


    uint32_t get_memory_end() {
        const Elf32_Phdr *last = reinterpret_cast<const Elf32_Phdr *>(
                elf.data() + hdr->e_phoff + hdr->e_phentsize * (hdr->e_phnum - 1));

        return last->p_vaddr + last->p_memsz;
    }

    uint32_t get_heap_addr() {
        // return first 8 byte aligned address after the memory image
        auto s = get_memory_end();
        return s + s % 8;
    }

    uint32_t get_entrypoint() {
        return hdr->e_entry;
    }


    const char *get_section_string_table() {
        assert (hdr->e_shoff != 0 && "string table section not available");

        const Elf32_Shdr *s = reinterpret_cast<const Elf32_Shdr *>(elf.data() + hdr->e_shoff + hdr->e_shentsize*hdr->e_shstrndx);  // 找到节头表->找到节名字表表项；
        const char *start = elf.data() + s->sh_offset; // 找到节名字表
        return start;
    }


    const char *get_symbol_string_table() { //< 找到符号表（符号表是ELF文件中诸多个节中的一个）
        auto s = get_section(".strtab");    //< 符号表包含诸多表项，Elf32_Sym结构描述了这些表项
        return elf.data() + s->sh_offset; 
    }


    const Elf32_Sym *get_symbol(const char *symbol_name) {
        const Elf32_Shdr *s = get_section(".symtab");  //找到ELF文件中节头表中的符号表表项
        const char *strings = get_symbol_string_table(); //找到ELF文件中的*节名字表*

        assert (s->sh_size % sizeof(Elf32_Sym) == 0); //

        auto num_entries = s->sh_size / sizeof(Elf32_Sym);
        for (unsigned i=0; i<num_entries; ++i) { //< 遍历符号表中的表项
            const Elf32_Sym *p = reinterpret_cast<const Elf32_Sym *>(elf.data() + s->sh_offset + i*sizeof(Elf32_Sym));

            //std::cout << "check symbol: " << strings + p->st_name << std::endl;

            if (!strcmp(strings + p->st_name, symbol_name)) {
                return p;
            }
        }

        throw std::runtime_error("unable to find symbol in the symbol table");
    }


    uint32_t get_begin_signature_address() {
        auto p = get_symbol("begin_signature");
        return p->st_value;
    }

    uint32_t get_end_signature_address() {
        auto p = get_symbol("end_signature");
        return p->st_value;
    }


    const Elf32_Shdr *get_section(const char *section_name) { //< 找到节头表中相关的节
        if (hdr->e_shoff == 0) {
            throw std::runtime_error("unable to find section address, section table not available");
        }

        const char *strings = get_section_string_table(); //< 找到节名字表(它也是ELF文件中多个节的其中一个)

        for (unsigned i=0; i<hdr->e_shnum; ++i) {
            const Elf32_Shdr *s = reinterpret_cast<const Elf32_Shdr *>( //< 对节进行遍历
                    elf.data() + hdr->e_shoff + hdr->e_shentsize*i);

            //std::cout << "check section: " << strings + s->sh_name << std::endl;

            if (!strcmp(strings + s->sh_name, section_name)) {
                return s; 
            }
        }

        throw std::runtime_error("unable to find section address, seems not available");
    }
};


#endif //RISCV_ISA_ELF_LOADER_H
