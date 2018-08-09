// Copyright (C) 2018 Woods Hole Oceanographic Institution
//
// This file is part of the WHOI Innovation Project (WIP) Comms Project
// ("wip-comms").
//
// wip-comms is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// wip-comms is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with wip-comms in the COPYING.md file at the project root.
// If not, see <http://www.gnu.org/licenses/>.

#ifndef LIAISONIMAGERY20180801H
#define LIAISONIMAGERY20180801H

#include <Wt/WText>
#include <Wt/WCssDecorationStyle>
#include <Wt/WBorder>
#include <Wt/WColor>
#include <Wt/WVBoxLayout>

#include "wip-comms/config/liaison_config.pb.h"

#include "goby/middleware/liaison/liaison_container.h"
#include "goby/middleware/multi-thread-application.h"

#include "progressive_imagery/goby/groups.h"
#include "progressive_imagery/progressive_imagery.pb.h"

namespace wip
{
    class ImageryCommsThread;
    
    class LiaisonImagery : public goby::common::LiaisonContainerWithComms<LiaisonImagery,
        ImageryCommsThread>
    {
    public:
        LiaisonImagery(const goby::common::protobuf::LiaisonConfig& cfg);

        void handle_updated_image(const dsl::protobuf::UpdatedImageEvent& event);
        
    private:
        const protobuf::LiaisonImageryConfig& imagery_cfg_;
        
        Wt::WVBoxLayout* main_layout_;
        Wt::WContainerWidget* image_container_;

        std::map<int, Wt::WImage*> images_;
        
    };
    
     
    class ImageryCommsThread : public goby::common::LiaisonCommsThread<LiaisonImagery>
    {
    public:
    ImageryCommsThread(LiaisonImagery* wt_app, const goby::common::protobuf::LiaisonConfig& config, int index) :
        goby::common::LiaisonCommsThread<LiaisonImagery>(wt_app, config, index),
            wt_app_(wt_app)
            {
                interprocess().subscribe<dsl::progressive_imagery::groups::updated_image,
                    dsl::protobuf::UpdatedImageEvent>(
                        [this](const dsl::protobuf::UpdatedImageEvent& updated_image)
                        {
                            wt_app_->post_to_wt(
                                [=]() { wt_app_->handle_updated_image(updated_image); });   
                        });
                
            }
            ~ImageryCommsThread()
            {
            }
            
        private:
            friend class LiaisonImagery;
            LiaisonImagery* wt_app_;
            
        };
    
    
}

#endif
