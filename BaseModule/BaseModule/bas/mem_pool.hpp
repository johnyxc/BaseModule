#ifndef __MEM_POOL_HPP_2015_07_01__
#define __MEM_POOL_HPP_2015_07_01__
//	基础模块内存池实现
//	内存池无法避免内存碎片
//	需要在分配的时候做最佳匹配，以尽量节省空间
//	内存池无法处理一次性分配超过10M的请求
#include <bio.hpp>
#include <vector>
#include <map>

namespace bas
{
	namespace detail
	{
		static const int block_size = 10 * 1024 * 1024;		//	10M

		//	mem_pool_manager_t 管理的最小粒度
		struct block_t : bio_bas_t<block_t>
		{
			//	可以分配的最小粒度
			struct alloc_unit
			{
				int offset;			//	距离块起始位置的偏移
				int size;			//	本单元块大小
				char bfree;			//	是否空闲单元块
				alloc_unit* prev;	//	上一块单元块位置
				alloc_unit* next;	//	下一块单元块位置
			};

		public :
			block_t() : buf_(), total_size_(), free_count_(), head_() { i_init(); }
			~block_t() { i_uninit(); }

		public :
			//	TODO : 此处最好做最佳匹配，而不是随意分配一个足够大的单元块
			void* alloc_buffer(int size)
			{
				alloc_unit* cur = head_;
				alloc_unit* prev = cur;
				//	查找满足条件的一个单元块
				while(!cur->bfree && cur->size < size)
				{
					prev = cur;
					cur = cur->next;
				}

				int offset = cur - head_;
				alloc_unit* next = cur + size + sizeof(alloc_unit);

				if(next == head_ + total_size_)
				{	//	直接分配完整个内存块
					i_write_head_info(cur, offset, size, 0, prev, 0);
					free_count_--;
				}
				else
				{	//	未分配完
					if(total_size_ - offset <= sizeof(alloc_unit))
					{	//	剩余空间不足
						i_write_head_info(cur, offset, size, 0, prev, 0);
						free_count_--;
					}
					else
					{
						int next_size = cur->size - size - sizeof(alloc_unit);
						alloc_unit* temp_next = cur->next;

						//	更新本块信息
						i_write_head_info(cur, offset, size, 0, prev, next);

						//	更新下一块信息
						i_write_head_info(next, next - head_, next_size, 1, cur, temp_next);
					}
				}

				return cur + sizeof(alloc_unit);
			}

			void free_buffer(void* buf)
			{
				if(!buf ||
					buf < head_ + sizeof(alloc_unit) ||
					buf > head_ + total_size_) return;

				alloc_unit* cur = (alloc_unit*)((char*)buf - sizeof(alloc_unit));
				cur->bfree = 1;
				i_merge_unit(cur);
			}

			int get_total_size() { return total_size_; }
			int get_free_count() { return free_count_; }

		private :
			void i_init()
			{
				buf_ = new char[block_size];
				total_size_ = block_size;

				i_write_head_info(buf_, 0, total_size_, 1, 0, 0);
				head_ = (alloc_unit*)buf_;

				free_count_ = 1;
			}

			void i_uninit()
			{
				if(buf_) delete buf_;
			}

			void i_write_head_info(void* p, int offset, int size, char bfree, alloc_unit* prev, alloc_unit* next)
			{
				if(!p) return;
				alloc_unit* u = (alloc_unit*)p;
				u->offset = offset;
				u->size	  = size;
				u->bfree  = bfree;
				u->prev   = prev;
				u->next   = next;
			}

			void i_merge_unit(alloc_unit* cur)
			{	//	合并单元块，一般情况下发生在释放的时候
				//	只处理当前空闲的单元块，使用中的单元块不做任何处理
				if(cur->prev == 0 && cur->next == 0) return;
				if(cur->bfree == 0) return;

				if(cur->prev->bfree && cur->next->bfree) {
					free_count_--;
				} else if(!cur->prev->bfree && !cur->next->bfree) {
					free_count_++;
				}

				while(cur->prev && cur->prev->bfree) cur = cur->prev;
				while(cur->next && cur->next->bfree)
				{
					cur->size = cur->size + cur->next->size + sizeof(alloc_unit);
					cur->next = cur->next->next;
				}

				if(cur->next)
				{
					cur->next->prev = cur;
				}
			}

		private :
			void*	buf_;			//	缓冲区
			int		total_size_;	//	本块总大小
			int		free_count_;	//	未使用单元块数量
			alloc_unit* head_;		//	头
		};

		//////////////////////////////////////////////////////////////////////////
		//	内存池管理
		struct mem_pool_manager_t : bio_bas_t<mem_pool_manager_t>
		{
		public :
			mem_pool_manager_t() {}
			~mem_pool_manager_t() {}

		public :
			void init(int count = 1)
			{
				block_t* block = new block_t;
				block_list_.push_back(block);
			}

			void* alloc(int size)
			{
				void* buf = 0;
				
				for(int i = 0; i < block_list_.size(); i++)
				{
					block_t* block = block_list_[i];
					if(block->get_free_count() == 0) continue;
					buf = block->alloc_buffer(size);
					buf_block_map_.insert(std::pair<void*, block_t*>(buf, block));
					break;
				}

				if(buf == 0)
				{
					block_t* block = new block_t;
					block_list_.push_back(block);
					buf = block->alloc_buffer(size);
				}

				return buf;
			}

			void free(void* buf)
			{
				if(!buf) return;
				std::map<void*, block_t*>::const_iterator iter;
				iter = buf_block_map_.find(buf);
				if(iter == buf_block_map_.end()) return;
				iter->second->free_buffer(buf);
			}

		private :

		private :
			std::vector<block_t*> block_list_;
			std::map<void*, block_t*> buf_block_map_;
		};
	}
}

#endif
