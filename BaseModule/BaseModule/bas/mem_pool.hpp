#ifndef __MEM_POOL_HPP_2015_07_01__
#define __MEM_POOL_HPP_2015_07_01__
#pragma warning(disable : 4996)
/*
	*����ģ���ڴ��ʵ��
	*�ڴ���޷������ڴ���Ƭ
	*��Ҫ�ڷ����ʱ�������ƥ�䣬�Ծ�����ʡ�ռ�
	*�ڴ���޷�����һ���Է��䳬��10M������

	*�ڴ浥Ԫ��ģ�ͣ�
	---------	----
	| ��Ԫͷ	|		| 20B
	---------	----|
	| ������	|		|
	| 		|		|
	|		|		| 10M
	---------		|
	| ��Ԫͷ	|		|
	---------		|
	| ������	|		|
	|		|		|
	---------	----
*/
#include <bio.hpp>
#include <auto_ptr.hpp>
#include <osfunc.hpp>
#include <limits.h>
#include <vector>
#include <map>

namespace bas
{
	namespace detail
	{
		static const int block_size = 10 * 1024 * 1024;		//	10M

		//	mem_pool_manager_t �������С����
		struct block_t : bio_bas_t<block_t>
		{
			//	���Է������С����
			struct alloc_unit
			{
				int offset;			//	�������ʼλ�õ�ƫ��
				int size;			//	����Ԫ���С
				int bfree;			//	�Ƿ����
				alloc_unit* prev;	//	��һ�鵥Ԫ��λ��
				alloc_unit* next;	//	��һ�鵥Ԫ��λ��
			};

		public :
			block_t() : buf_(), total_size_(), free_count_(), head_() { i_init(); }
			~block_t() { i_uninit(); }

		public :
			void* alloc_buffer(int size)
			{
				alloc_unit* cur = head_;
				alloc_unit* prev = cur;

				//	�˴����ܷ���ʧ��
				best_match(cur, prev, size);
				if(!cur) return 0;

				int offset = (char*)cur - (char*)head_;
				int cur_len = cur->size;
				alloc_unit* next = (alloc_unit*)((char*)cur + size + sizeof(alloc_unit));

				if(cur_len <= size + sizeof(alloc_unit))
				{	//	ʣ��ȫ��������˿�
					i_write_head_info(cur, offset, cur->size, 0, prev, cur->next);
					--free_count_;
				}
				else
				{
					alloc_unit* temp_next = cur->next;
					int next_size = cur->size - size - sizeof(alloc_unit);

					//	���±�����Ϣ
					i_write_head_info(cur, offset, size, 0, prev, next);

					//	������һ����Ϣ
					i_write_head_info(next, (char*)next - (char*)head_, next_size, 1, cur, temp_next);
				}

				return (char*)cur + sizeof(alloc_unit);
			}

			void free_buffer(void* buf)
			{
				if(!buf ||
					buf < (char*)head_ + sizeof(alloc_unit) ||
					buf > (char*)head_ + total_size_) return;

				alloc_unit* cur = (alloc_unit*)((char*)buf - sizeof(alloc_unit));
				cur->bfree = 1;
				++free_count_;
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
				u->prev   = (u == prev) ? 0 : prev;
				u->next   = next;
			}

			void i_merge_unit(alloc_unit* cur)
			{	//	�ϲ���Ԫ�飬һ������·������ͷŵ�ʱ��
				//	ֻ����ǰ���еĵ�Ԫ�飬ʹ���еĵ�Ԫ�鲻���κδ���
				if(cur->prev == 0 && cur->next == 0) return;
				if(cur->bfree == 0) return;

				while(cur->prev && cur->prev->bfree) cur = cur->prev;
				while(cur->next)
				{
					if(cur->next->bfree) {
						cur->size = cur->size + cur->next->size + sizeof(alloc_unit);
						--free_count_;
					} else {
						break;
					}
					cur->next = cur->next->next;
					if(cur->next) cur->next->prev = cur;
				}

				if(cur->next)
				{
					cur->next->prev = cur;
				}
			}

			void best_match(alloc_unit*& cur, alloc_unit*& prev, int size)
			{
				if(!cur) return;

				//	��������������һ����Ԫ��
				//	�����õ������ƥ����ԣ�Ҫ���С�Ϳ��е�Ԫ���С����һ��
				//	���ܴ���Ч������
				int alpha = INT_MAX;
				alloc_unit* tmp = cur;
				alloc_unit* bm = 0;
				while(tmp)
				{
					if(tmp->bfree && tmp->size >= size)
					{
						int gap = tmp->size - size;
						if(gap < alpha)
						{
							alpha = gap;
							bm = tmp;
						}
						if(alpha == 0) break;
					}
					tmp = tmp->next;
				}

				if(bm) {
					cur = bm;
					prev = cur->prev;
				} else {
					cur = 0;
					prev = 0;
				}
			}

		private :
			void*	buf_;			//	������
			int		total_size_;	//	�����ܴ�С
			int		free_count_;	//	δʹ�õ�Ԫ������
			alloc_unit* head_;		//	ͷ
		};

		//////////////////////////////////////////////////////////////////////////
		//	�ڴ�ع���
		struct mem_pool_manager_t : bio_bas_t<mem_pool_manager_t>
		{
		public :
			mem_pool_manager_t() : mutex_() { mutex_ = get_mutex(); }
			~mem_pool_manager_t() { if(mutex_) release_mutex(mutex_); }

		public :
			void init(int count = 1)
			{
				block_t* block = new block_t;
				block_list_.push_back(block);
			}

			void uninit()
			{
				for(unsigned int i = 0; i < block_list_.size(); i++)
				{
					block_t* block = block_list_[i];
					if(block) delete block;
				}
			}

			void* alloc(int size)
			{
				void* buf = 0;
				block_t* block = 0;

				lock(mutex_);
				for(unsigned int i = 0; i < block_list_.size(); i++)
				{
					block = block_list_[i];
					if(block->get_free_count() == 0) continue;
					buf = block->alloc_buffer(size);
					break;
				}

				if(buf == 0)
				{
					block = new block_t;
					block_list_.push_back(block);
					buf = block->alloc_buffer(size);
				}

				if(buf) buf_block_map_.insert(std::pair<void*, block_t*>(buf, block));
				unlock(mutex_);

				return buf;
			}

			bool free(void* buf)
			{
				if(!buf) return false;

				lock(mutex_);
				std::map<void*, block_t*>::const_iterator iter;
				iter = buf_block_map_.find(buf);
				if(iter == buf_block_map_.end()) { unlock(mutex_); return false; }

				iter->second->free_buffer(buf);
				buf_block_map_.erase(iter);
				unlock(mutex_);

				return true;
			}

		private :
			std::vector<block_t*> block_list_;
			std::map<void*, block_t*> buf_block_map_;
			HMUTEX mutex_;
		};
	}

	detail::auto_ptr<detail::mem_pool_manager_t> mem_pool = make_auto_ptr<detail::mem_pool_manager_t>();
}

#endif
